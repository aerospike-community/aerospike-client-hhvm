#include "hphp/runtime/ext/extension.h"
#include "hphp/runtime/base/execution-context.h"
#include "hphp/runtime/vm/native-data.h"
#include "conversions.h"
#include "helper.h"
#include "class_constants.h"
#include "ext_aerospike.h"
#include "policy.h"
#include "batch_op_manager.h"
#include "scan_operation.h"
#include "udf_operations.h"

#include "hphp/runtime/base/builtin-functions.h"
#include "aerospike/as_bytes.h"
#include "hphp/runtime/vm/vm-regs.h"

namespace HPHP {

    /*
     * Aerospike extension globals
     */
    std::unordered_map<std::string, aerospike_ref *> persistent_list;
    pthread_rwlock_t connection_mutex;
    pthread_rwlock_t scan_query_callback_mutex;

	ini_entries ini_entry;

    /*
     * Static member's definition
     */
    IMPLEMENT_REQUEST_LOCAL(AerospikeRequestLocals, Aerospike::locals);

    /*
     ************************************************************************************
     * Definitions of Member functions of Class Aerospike declared in
     * ext_aerospike.h
     ************************************************************************************
     */

    /*
     ************************************************************************************
     * Constructor
     ************************************************************************************
     */
    Aerospike::Aerospike() 
    {
        pthread_rwlock_init(&latest_error_mutex, NULL);
    }

    /*
     ************************************************************************************
     * Function to close an open aerospike C client's connection when ref count
     * reaches 0.
     * Also destroys the connection object and resets flags.
     ************************************************************************************
     */
    void Aerospike::sweep()
    {
        as_error error;

        as_error_init(&error);

        if (as_ref_p) {
            if (is_connected) {
                pthread_rwlock_wrlock(&connection_mutex);
                as_ref_p->ref_php_object--;
                pthread_rwlock_unlock(&connection_mutex);
                is_connected = false;

                if (!is_persistent) {
                    aerospike_close(as_ref_p->as_p, &error);
                    aerospike_destroy(as_ref_p->as_p);
                    as_ref_p->as_p = NULL;
                    free(as_ref_p);
                }
            } else {
                if (!is_persistent) {
                    aerospike_destroy(as_ref_p->as_p);
                    as_ref_p->as_p = NULL;
                    free(as_ref_p);
                }
            }
            as_ref_p = NULL;
        }
    }

    /*
     ************************************************************************************
     * Destructor
     ************************************************************************************
     */
    Aerospike::~Aerospike()
    {
        sweep();
    }

    /*
     ************************************************************************************
     * This macro will create key in the unordered_map.
     * Which is host_entry => address:port.
     ************************************************************************************
     */
namespace {
    inline char* create_new_alias(const as_config& config, int iter_hosts) {
        assert(iter_hosts >= 0);
        assert(iter_hosts < config.hosts->size);

        as_host* const host = (as_host*)as_vector_get(config.hosts, iter_hosts);
        auto const addr_len = strlen(host->name);
        auto const ret_size = addr_len + 1 + MAX_PORT_SIZE + 1;
        auto ret = static_cast<char*>(malloc(ret_size));

        snprintf(ret, ret_size, "%s:%d", host->name, host->port);
        return ret;
    }
}

    /*
     ************************************************************************************
     * This function will iterate over remaining list of hosts and hash them
     * into the persistent list, each pointing to the same aerospike ref object.
     * Increment corresponding ref_host_entry within the aerospike_ref object.
     ************************************************************************************
     */
    void Aerospike::iterate_hosts_add_entry(as_config& config, int matched_host_id) {
        char                *alias_to_search = NULL;
        int                 iter_hosts;

        for (iter_hosts = 0; iter_hosts < config.hosts->size; iter_hosts++) {
            if (iter_hosts == matched_host_id) {
                continue;
            }
            alias_to_search = create_new_alias(config, iter_hosts);
            /*
             * Write lock is required as we are modifying as_ref_p and which is
             * pointing to one of the entry in the persistent_list.
             */
            pthread_rwlock_wrlock(&connection_mutex);
            as_ref_p->ref_host_entry++;
            persistent_list[alias_to_search] = as_ref_p;
            pthread_rwlock_unlock(&connection_mutex);

            if (alias_to_search) {
                free(alias_to_search);
                alias_to_search = NULL;
            }
        }
    }

    /*
     ************************************************************************************
     * This function will create new host entry.
     * Initialize it's fields with the appropriate values.
     ************************************************************************************
     */
    void Aerospike::create_new_host_entry(as_config& config, as_error& error)
    {
        as_ref_p = (aerospike_ref *) malloc(sizeof(aerospike_ref));
        if (as_ref_p == NULL) {
            as_error_update(&error, AEROSPIKE_ERR_CLIENT, "memory allocation failed");
            return;
        }
        as_ref_p->as_p = NULL;
        as_ref_p->ref_host_entry = 0;
        as_ref_p->ref_php_object = 1;
        as_ref_p->as_p = aerospike_new(&config);
    }

    /*
     ************************************************************************************
     * This function will configure the connection.
     * i.e. Creating new host entry in the persistent list, If host is not present.
     * And reuse the same connection if hot is already present in the persistent list.
     ************************************************************************************
     */
    as_status Aerospike::configure_connection(as_config& config, as_error& error)
    {
        char                *alias_to_search = NULL;
        int                 iter_hosts;
        int                 matched_host_id = -1;
        aerospike_ref       *host_entry = NULL;

        as_error_reset(&error);

        if (is_persistent) {
            for (iter_hosts = 0; iter_hosts < config.hosts->size; iter_hosts++) {
                alias_to_search = create_new_alias(config, iter_hosts);
                pthread_rwlock_rdlock(&connection_mutex);
                if (persistent_list[alias_to_search] != nullptr) {
                    host_entry = persistent_list[alias_to_search];
                    as_ref_p = host_entry;
                    pthread_rwlock_unlock(&connection_mutex);
                    is_connected = true;
                    pthread_rwlock_wrlock(&connection_mutex);
                    host_entry->ref_php_object++;
                    pthread_rwlock_unlock(&connection_mutex);
                    matched_host_id = iter_hosts;
                    iterate_hosts_add_entry(config, matched_host_id);
                } else {
                    pthread_rwlock_unlock(&connection_mutex);
                }

                if (alias_to_search) {
                    free(alias_to_search);
                    alias_to_search = NULL;
                }
                if (matched_host_id != -1) {
                    return error.code;
                }
            }

            alias_to_search = config.hosts->size ? create_new_alias(config, 0) : NULL;
            create_new_host_entry(config, error);
            if (error.code == AEROSPIKE_OK) {
                as_ref_p->ref_host_entry++;
                host_entry = as_ref_p;
                pthread_rwlock_wrlock(&connection_mutex);
                persistent_list[alias_to_search] = host_entry;
                pthread_rwlock_unlock(&connection_mutex);

                if (alias_to_search) {
                    free(alias_to_search);
                    alias_to_search = NULL;
                }

                iterate_hosts_add_entry(config, 0);
            }
        } else {
            create_new_host_entry(config, error);
        }

        return error.code;
    }

    /*
     ************************************************************************************
     * Updates the "lastest_error" field in the object's NativeData
     * May block while waiting for shared mutex
     ************************************************************************************
     */
    void Aerospike::setError(const as_error& error) {
        pthread_rwlock_wrlock(&latest_error_mutex);
        /* https://github.com/aerospike/aerospike-client-c/pull/33
         * Obviates the need for this const cast.
         */
        as_error_copy(&latest_error, const_cast<as_error*>(&error));
        pthread_rwlock_unlock(&latest_error_mutex);
    }

    /*
     ************************************************************************************
     * Definitions of Native methods in PHP Aerospike class declared in
     * ext_aerospike.h and specified in ext_aerospike.php
     ************************************************************************************
     */

    /* {{{ proto Aerospike::__construct(array config [, bool persistent_connection=true [, array options]]))
      Creates a new Aerospike object, with optional persistent connection control */
    void HHVM_METHOD(Aerospike, __construct, const Array& php_config,
            bool persistent_connection, const Variant& options)
    {
        VMRegAnchor         _;
        auto                data = Native::data<Aerospike>(this_);
        as_error            error;
        as_config           config;
        PolicyManager       policy_manager(&config);

        as_error_init(&error);

        data->is_persistent = persistent_connection;
        data->serializer_value = SERIALIZER_PHP;

        if (data->as_ref_p) {
            as_error_update(&error, AEROSPIKE_ERR_CLIENT,
                    "Connection already exists!");
        } else if (AEROSPIKE_OK == php_config_to_as_config(php_config,
                    config, error)) {
            if (AEROSPIKE_OK == policy_manager.set_global_defaults(&data->serializer_value,
                        options, error)) {
                if (AEROSPIKE_OK == data->configure_connection(config, error)) {
                    if (data->as_ref_p->ref_php_object <= 1 && data->as_ref_p->as_p) {
                        if (AEROSPIKE_OK == aerospike_connect(data->as_ref_p->as_p,
                                    &error)) {
                            data->is_connected = true;
                        } else {
                            as_error_update(&error, AEROSPIKE_ERR_CLUSTER,
                                    "Unable to connect to server");
                            data->as_ref_p->ref_php_object--;
                        }
                    }
                }
            }
        }

        data->setError(error);
    }
    /* }}} */

    /* {{{ proto bool Aerospike::isConnected( void )
       Tests whether the connection to the cluster was established */
    bool HHVM_METHOD(Aerospike, isConnected)
    {
        auto                data = Native::data<Aerospike>(this_);

        if (!data->as_ref_p || !data->as_ref_p->as_p) {
            // Invalid aerospike connection object
            return false;
        }

        return data->is_connected;
    }
    /* }}} */

    /* {{{ proto Aerospike::close( void )
       Closes all connections to the cluster */
    int64_t HHVM_METHOD(Aerospike, close)
    {
        VMRegAnchor         _;
        auto                data = Native::data<Aerospike>(this_);
        as_error            error;

        as_error_init(&error);

        if (!data->as_ref_p || !data->as_ref_p->as_p) {
            as_error_update(&error, AEROSPIKE_ERR_CLIENT,
                    "Invalid aerospike connection object");
        } else if (!data->is_connected ||
                data->as_ref_p->ref_php_object < 1) {
            as_error_update(&error, AEROSPIKE_ERR_CLIENT,
                    "Already disconnected!");
        } else {
            if (data->is_persistent == false) {
                aerospike_close(data->as_ref_p->as_p, &error);
                data->as_ref_p->ref_php_object = 0;
            } else {
                /*
                 * Decrements ref_php_object which indicates the no. of
                 * references for internal CSDK aerospike object being held by
                 * the various PHP userland functions.
                 */
                pthread_rwlock_wrlock(&connection_mutex);
                data->as_ref_p->ref_php_object--;
                pthread_rwlock_unlock(&connection_mutex);
            }
            data->is_connected = false;
        }

        data->setError(error);
        return error.code;
    }
    /* }}} */

    /* {{{ proto Aerospike::reconnect( void )
       Closes then reopens all connections to the cluster */
    int64_t HHVM_METHOD(Aerospike, reconnect)
    {
        VMRegAnchor         _;
        auto                data = Native::data<Aerospike>(this_);
        as_error            error;

        as_error_init(&error);

        if (!data->as_ref_p || !data->as_ref_p->as_p) {
            as_error_update(&error, AEROSPIKE_ERR_CLIENT,
                    "Invalid aerospike connection object");
        } else if (data->is_connected ||
                data->as_ref_p->ref_php_object >= 1) {
            as_error_update(&error, AEROSPIKE_ERR_CLIENT,
                    "Already connected!");
        } else {
            if (data->is_persistent == false) {
                if (AEROSPIKE_OK == aerospike_connect(data->as_ref_p->as_p, &error)) {
                    data->as_ref_p->ref_php_object = 1;
                    data->is_connected = true;
                }
            } else {
                /*
                 * Cluster is valid or not this condition will
                 * indicate whether connect was successful or not
                 * on that connection incase of persistent connection.
                 */
                if (data->as_ref_p->as_p->cluster) {
                    /*
                     * Decrements ref_php_object which indicates the no. of
                     * references for internal CSDK aerospike object being held by
                     * the various PHP userland functions.
                     */
                    pthread_rwlock_wrlock(&connection_mutex);
                    data->as_ref_p->ref_php_object++;
                    pthread_rwlock_unlock(&connection_mutex);
                    data->is_connected = true;
                } else {
                    as_error_update(&error, AEROSPIKE_ERR_CLIENT,
                            "Trying to connect invalid persistent connection");
                }
            }
        }

        data->setError(error);
        return error.code;
    }
    /* }}} */

    /* {{{ proto static Aerospike::setSerializer( callback serialize_cb )
       Sets a userland method as responsible for serializing bin values */
    bool HHVM_STATIC_METHOD(Aerospike, setSerializer, const Variant& callback)
    {
        if (!callback.isObject()) {
            //Invalid callback function
            return false;
        }

        Aerospike::setSerializer(callback);
        return true;
    }
    /* }}} */

    /* {{{ proto static Aerospike::setDeserializer( callback unserialize_cb )
       Sets a userland method as responsible for deserializing bin values */
    bool HHVM_STATIC_METHOD(Aerospike, setDeserializer, const Variant& callback)
    {
        if (!callback.isObject()) {
            //Invalid callback function
            return false;
        }

        Aerospike::setDeserializer(callback);
        return true;
    }
    /* }}} */

    /* {{{ proto int Aerospike::put( array key, array record [, int ttl=0 [, array options ]] )
       Writes a record to the cluster */
    int64_t HHVM_METHOD(Aerospike, put, const Array& php_key,
            const Array& php_rec, int64_t ttl,
            const Variant& options)
    {
        VMRegAnchor         _;
        auto                data = Native::data<Aerospike>(this_);
        as_error            error;
        as_key              key;
        as_record           rec;
        StaticPoolManager   static_pool;
        as_policy_write     write_policy;
        bool                key_initialized = false;
        int16_t             serializer_option = 0;
        PolicyManager       policy_manager;

        as_error_init(&error);

        if (!data->as_ref_p || !data->as_ref_p->as_p) {
            as_error_update(&error, AEROSPIKE_ERR_CLIENT,
                    "Invalid aerospike connection object");
        } else if (!data->is_connected) {
            as_error_update(&error, AEROSPIKE_ERR_CLUSTER,
                    "put: connection not established");
        } else if (AEROSPIKE_OK == php_key_to_as_key(php_key, key, error)) {
            key_initialized = true;
            if (AEROSPIKE_OK == policy_manager.initPolicyManager(&write_policy,
                        "write", &data->as_ref_p->as_p->config, error) &&
                    AEROSPIKE_OK == policy_manager.set_policy(&serializer_option,
                        data->serializer_value, options, error)) {
                if (AEROSPIKE_OK == php_record_to_as_record(php_rec, rec,
                            ttl, static_pool, serializer_option, error)) {
                    policy_manager.set_generation_value(&rec.gen, options,
                            error);
                    aerospike_key_put(data->as_ref_p->as_p, &error,
                            &write_policy, &key, &rec);
                    as_record_destroy(&rec);
                }
            }
        }

        if (key_initialized) {
            as_key_destroy(&key);
        }
        data->setError(error);
        return error.code;
    }
    /* }}} */

    /* {{{ proto int Aerospike::addIndex( string ns, string set, string bin,
     * string name, int index_type, int data_type, array options)
     * Creates a secondary index on given params.
     */
    int64_t HHVM_METHOD(Aerospike, addIndex, const Variant &ns, const Variant &set, 
            const Variant &bin, const Variant &name, const Variant &index_type, 
            const Variant &data_type, const Variant &options)
    {
        auto            data = Native::data<Aerospike>(this_);
        as_error        error;
        as_index_task   task;
        as_policy_info  info_policy;
        as_status       aerospike_status;
        PolicyManager   policy_manager;
       
        as_error_init(&error);

        if (!data->as_ref_p || !data->as_ref_p->as_p) {
            as_error_update(&error, AEROSPIKE_ERR_CLIENT,
                    "Invalid aerospike connection object");
        } else if (!data->is_connected) {
            as_error_update(&error, AEROSPIKE_ERR_CLUSTER,
                    "addIndex: connection not established");
        } else if (AEROSPIKE_OK == policy_manager.initPolicyManager(&info_policy,
                    "info", &data->as_ref_p->as_p->config, error) &&
                AEROSPIKE_OK == policy_manager.set_policy(NULL,
                    data->serializer_value, options, error)) {
            aerospike_status = aerospike_index_create_complex(data->as_ref_p->as_p, 
                        &error, &task, &info_policy, ns.toString().c_str(), set.toString().c_str(), 
                        bin.toString().c_str(), name.toString().c_str(), 
                        (as_index_type)index_type.toInt64(), (as_index_datatype)data_type.toInt64());
            if(aerospike_status != AEROSPIKE_OK){
                aerospike_index_create_wait(&error, &task, 0);
            }
        }

        data->setError(error);
        return error.code;
    }
    /* }}} */
    
    /* {{{ proto int Aerospike::dropIndex( string ns, string name, array options)
     * Removes the secodary index created.
     */
    int64_t HHVM_METHOD(Aerospike, dropIndex, const Variant &ns, const Variant &name, const Variant &options)
    {
        auto            data = Native::data<Aerospike>(this_);
        as_error        error;
        as_policy_info  info_policy;
        PolicyManager   policy_manager;

        as_error_init(&error);

        if (!data->as_ref_p || !data->as_ref_p->as_p) {
            as_error_update(&error, AEROSPIKE_ERR_CLIENT,
                    "Invalid aerospike connection object");
        } else if (!data->is_connected) {
            as_error_update(&error, AEROSPIKE_ERR_CLUSTER,
                    "dropIndex:: connection not established");
        } else if (AEROSPIKE_OK == policy_manager.initPolicyManager(&info_policy,
                    "info", &data->as_ref_p->as_p->config, error) &&
                AEROSPIKE_OK == policy_manager.set_policy(NULL,
                    data->serializer_value, options, error)){
            aerospike_index_remove( data->as_ref_p->as_p, 
                        &error, &info_policy,ns.toString().c_str(), name.toString().c_str());
        }
        
        data->setError(error);
        return error.code;
    }
    /* }}} */

    /* {{{ proto int Aerospike::get( array key, array record [, array filter [, array options]] )
       Reads a record from the cluster */
    int64_t HHVM_METHOD(Aerospike, get, const Array& php_key, VRefParam php_rec,
            const Variant& filter_bins, const Variant& options)
    {
        VMRegAnchor         _;
        auto                data = Native::data<Aerospike>(this_);
        as_status           status = AEROSPIKE_OK;
        as_error            error;
        as_key              key;
        as_record           *rec_p = NULL;
        as_policy_read      read_policy;
        bool                key_initialized = false;
        PolicyManager       policy_manager;

        if (!data->as_ref_p || !data->as_ref_p->as_p) {
            as_error_update(&error, AEROSPIKE_ERR_CLIENT,
                    "Invalid aerospike connection object");
        } else if (!data->is_connected) {
            as_error_update(&error, AEROSPIKE_ERR_CLUSTER,
                    "get: connection not established");
        } else if (AEROSPIKE_OK == php_key_to_as_key(php_key, key, error)) {
            key_initialized = true;
            if (AEROSPIKE_OK == policy_manager.initPolicyManager(&read_policy,
                        "read", &data->as_ref_p->as_p->config, error) &&
                    AEROSPIKE_OK == policy_manager.set_policy(NULL,
                        data->serializer_value, options, error)) {
                if (!filter_bins.isNull() && !filter_bins.isArray()) {
                    as_error_update(&error, AEROSPIKE_ERR_PARAM,
                            "Filter bins must be of type an Array");
                } else {
                    if (filter_bins.isArray()) {
                        status = aerospike_get_filtered_bins(filter_bins.toArray(),
                                data->as_ref_p->as_p, &read_policy, key,
                                &rec_p, error);
                    } else {
                        status = aerospike_key_get(data->as_ref_p->as_p, &error,
                                &read_policy, &key, &rec_p);
                    }
                    Array temp_php_rec = Array::Create();
                    if (status == AEROSPIKE_OK) {
                        as_record_to_php_record(rec_p, &key, temp_php_rec, &read_policy.key, error);
                    }
                    php_rec.assignIfRef(temp_php_rec);
                    as_record_destroy(rec_p);
                }
            }
        }
        
        if (key_initialized) {
            as_key_destroy(&key);
        }
        data->setError(error);
        return error.code;
    }
    /* }}} */

    /* {{{ proto int Aerospike::getMany( array keys, array &records [, array filter [, array options ]] )
       Returns a batch of records from the cluster */
    int64_t HHVM_METHOD(Aerospike, getMany, const Array& php_keys,
            VRefParam php_records, const Variant& filter_bins,
            const Variant& options)
    {
        VMRegAnchor         _;
        auto                data = Native::data<Aerospike>(this_);
        as_error            error;
        as_policy_batch     batch_policy;
        PolicyManager       policy_manager;

        as_error_init(&error);

        if (!data->as_ref_p || !data->as_ref_p->as_p) {
            as_error_update(&error, AEROSPIKE_ERR_CLIENT,
                    "Invalid aerospike connection object");
        } else if (!data->is_connected) {
            as_error_update(&error, AEROSPIKE_ERR_CLUSTER,
                    "getMany: connection not established");
        } else {
            try {
                BatchOpManager batch_op_manager(php_keys);
                if (AEROSPIKE_OK == policy_manager.initPolicyManager(&batch_policy,
                            "batch", &data->as_ref_p->as_p->config, error) &&
                        AEROSPIKE_OK == policy_manager.set_policy(NULL,
                            data->serializer_value, options, error)) {
                    Array   temp_php_records = Array::Create();
                    batch_op_manager.execute_batch_get(data->as_ref_p->as_p,
                            temp_php_records, filter_bins, batch_policy, error);
                    php_records.assignIfRef(temp_php_records);
                }
            } catch (const std::exception& e) {
                as_error_update(&error, AEROSPIKE_ERR_CLIENT,
                        "Failed to initialize batch operation");
            }
        }

        data->setError(error);
        return error.code;
    }
    /* }}} */

    /* {{{ proto int Aerospike::operate ( array key, array operations [, array &returned [, array options ]] )
       Performs multiple operations on a record */
    int64_t HHVM_METHOD(Aerospike, operate, const Array& php_key,
            const Array& php_operations, VRefParam returned,
            const Variant& options)
    {
        VMRegAnchor         _;
        auto                data = Native::data<Aerospike>(this_);
        as_error            error;
        as_key              key;
        StaticPoolManager   static_pool;
        as_operations       operations;
        as_record           *rec_p = NULL;
        as_policy_operate   operate_policy;
        int16_t             serializer_option = 0;
        bool                key_initialized = false;
        PolicyManager       policy_manager;

        as_error_init(&error);

        if (!data->as_ref_p || !data->as_ref_p->as_p) {
            as_error_update(&error, AEROSPIKE_ERR_CLIENT,
                    "Invalid aerospike connection object");
        } else if (!data->is_connected) {
            as_error_update(&error, AEROSPIKE_ERR_CLUSTER,
                    "operate: connection not established");
        } else if (AEROSPIKE_OK == php_key_to_as_key(php_key, key, error)) {
            key_initialized = true;
            if (AEROSPIKE_OK == policy_manager.initPolicyManager(&operate_policy,
                        "operate", &data->as_ref_p->as_p->config, error) &&
                    AEROSPIKE_OK == policy_manager.set_policy(&serializer_option,
                        data->serializer_value, options, error)) {
                if (AEROSPIKE_OK == php_operations_to_as_operations(php_operations,
                            operations, static_pool, serializer_option, error)) {
                    if (AEROSPIKE_OK == policy_manager.set_generation_value(&operations.gen,
                                options, error) && (AEROSPIKE_OK == policy_manager.set_ttl_value(&operations.ttl,
                                    options, error))) {
                        aerospike_key_operate(data->as_ref_p->as_p, &error,
                                &operate_policy, &key, &operations, &rec_p);
                        Array php_rec = Array::Create();
                        if (rec_p) {
                            bins_to_php_bins(rec_p, php_rec, error);
                            as_record_destroy(rec_p);
                        }
                        returned.assignIfRef(php_rec);
                    }
                }
            }
        }

        if (key_initialized) {
            as_key_destroy(&key);
        }
        data->setError(error);
        return error.code;
    }
    /* }}} */

    /* {{{ proto int Aerospike::remove( array key [, array options ] )
       Removes a record from the cluster */
    int64_t HHVM_METHOD(Aerospike, remove, const Array& php_key,
            const Variant& options)
    {
        VMRegAnchor         _;
        auto                data = Native::data<Aerospike>(this_);
        as_error            error;
        as_key              key;
        as_policy_remove    remove_policy;
        bool                key_initialized = false;
        PolicyManager       policy_manager;

        as_error_init(&error);

        if (!data->as_ref_p || !data->as_ref_p->as_p) {
            as_error_update(&error, AEROSPIKE_ERR_CLIENT,
                    "Invalid aerospike connection object");
        } else if (!data->is_connected) {
            as_error_update(&error, AEROSPIKE_ERR_CLUSTER,
                    "remove: connection not established");
        } else if (AEROSPIKE_OK == php_key_to_as_key(php_key, key, error)) {
            key_initialized = true;
            if (AEROSPIKE_OK == policy_manager.initPolicyManager(&remove_policy,
                        "remove", &data->as_ref_p->as_p->config, error) &&
                    AEROSPIKE_OK == policy_manager.set_policy(NULL,
                        data->serializer_value, options, error)) {
                policy_manager.set_generation_value(&remove_policy.generation,
                        options, error);
                aerospike_key_remove(data->as_ref_p->as_p, &error,
                        &remove_policy, &key);
            }
        }

        if (key_initialized) {
            as_key_destroy(&key);
        }
        data->setError(error);
        return error.code;
    }
    /* }}} */

    /* {{{ proto int Aerospike::removeBin( array key, array bins [, array options ])
       Removes a bin from a record */
    int64_t HHVM_METHOD(Aerospike, removeBin, const Array& php_key,
            const Array& bins, const Variant& options)
    {
        VMRegAnchor         _;
        auto                data = Native::data<Aerospike>(this_);
        as_error            error;
        as_key              key;
        as_record           record;
        as_policy_write     write_policy;
        bool                key_initialized = false;
        PolicyManager       policy_manager;

        as_error_init(&error);

        if (!data->as_ref_p || !data->as_ref_p->as_p) {
            as_error_update(&error, AEROSPIKE_ERR_CLIENT,
                    "Invalid aerospike connection object");
        } else if (!data->is_connected) {
            as_error_update(&error, AEROSPIKE_ERR_CLUSTER,
                    "removeBin: connection not established");
        } else if (AEROSPIKE_OK == php_key_to_as_key(php_key, key, error)) {
            key_initialized = true;
            if (AEROSPIKE_OK == policy_manager.initPolicyManager(&write_policy,
                        "write", &data->as_ref_p->as_p->config, error) &&
                    AEROSPIKE_OK == policy_manager.set_policy(NULL,
                        data->serializer_value, options, error)) {
                as_record_inita(&record, bins.size());
                if (AEROSPIKE_OK == policy_manager.set_generation_value(&record.gen,
                            options, error)) {
                    if (AEROSPIKE_OK == policy_manager.set_ttl_value(&record.ttl,
                                options, error)) {
                        if (AEROSPIKE_OK == set_nil_bins(&record, bins, error)) {
                            aerospike_key_put(data->as_ref_p->as_p, &error,
                                    &write_policy, &key, &record);
                        }
                    }
                }
                as_record_destroy(&record);
            }
        }

        if (key_initialized) {
            as_key_destroy(&key);
        }
        data->setError(error);
        return error.code;
    }
    /* }}} */

    /* {{{ proto int Aerospike::exists( array key, array &metadata [, array options] )
       Returns a record's metadata */
    int64_t HHVM_METHOD(Aerospike, exists, const Array& php_key,
            VRefParam metadata, const Variant& options)
    {
        VMRegAnchor         _;
        auto                data = Native::data<Aerospike>(this_);
        as_error            error;
        as_key              key;
        as_record           *record_p = NULL;
        as_policy_read      read_policy;
        bool                key_initialized = false;
        PolicyManager       policy_manager;

        as_error_init(&error);

        if (!data->as_ref_p || !data->as_ref_p->as_p) {
            as_error_update(&error, AEROSPIKE_ERR_CLIENT,
                    "Invalid aerospike connection object");
        } else if (!data->is_connected) {
            as_error_update(&error, AEROSPIKE_ERR_CLUSTER,
                    "exists: connection not established");
        } else if (AEROSPIKE_OK == php_key_to_as_key(php_key, key, error)) {
            key_initialized = true;
            if (AEROSPIKE_OK == policy_manager.initPolicyManager(&read_policy,
                        "read", &data->as_ref_p->as_p->config, error) &&
                    AEROSPIKE_OK == policy_manager.set_policy(NULL,
                        data->serializer_value, options, error)) {
                if (AEROSPIKE_OK == aerospike_key_exists(data->as_ref_p->as_p,
                            &error, &read_policy, &key, &record_p)) {
                    Array php_metadata = Array::Create();
                    metadata_to_php_metadata(record_p, php_metadata, error);
                    metadata.assignIfRef(php_metadata);
                }
                as_record_destroy(record_p);
            }
        }

        if (key_initialized) {
            as_key_destroy(&key);
        }
        data->setError(error);
        return error.code;
    }
    /* }}} */

    /* {{{ proto int Aerospike::existsMany( array keys, array &metadata [, array options] )
       Returns metadata for a batch of records with NULL for non-existent ones */
    int64_t HHVM_METHOD(Aerospike, existsMany, const Array& php_keys,
            VRefParam metadata, const Variant& options)
    {
        VMRegAnchor         _;
        auto                data = Native::data<Aerospike>(this_);
        as_error            error;
        as_policy_batch     batch_policy;
        PolicyManager       policy_manager;

        as_error_init(&error);

        if (!data->as_ref_p || !data->as_ref_p->as_p) {
            as_error_update(&error, AEROSPIKE_ERR_CLIENT,
                    "Invalid aerospike connection object");
        } else if (!data->is_connected) {
            as_error_update(&error, AEROSPIKE_ERR_CLUSTER,
                    "existsMany: connection not established");
        } else {
            try {
                BatchOpManager batch_op_manager(php_keys);
                if (AEROSPIKE_OK == policy_manager.initPolicyManager(&batch_policy,
                            "batch", &data->as_ref_p->as_p->config, error) &&
                        AEROSPIKE_OK == policy_manager.set_policy(NULL,
                            data->serializer_value, options, error)) {
                    Array   php_metadata = Array::Create();
                    batch_op_manager.execute_batch_exists(data->as_ref_p->as_p,
                            php_metadata, batch_policy, error);
                    metadata.assignIfRef(php_metadata);
                }
            } catch (const std::exception& e) {
                as_error_update(&error, AEROSPIKE_ERR_CLIENT,
                        "Failed to initialize batch operation");
            }
        }

        data->setError(error);
        return error.code;
    }
    /* }}} */

    /* {{{ proto string Aerospike::getKeyDigest( string ns, string set, int|string pk )
       Helper which computes the digest that for a given key */
    String HHVM_METHOD(Aerospike, getKeyDigest, const Variant& ns,
            const Variant& set, const Variant& primary_key)
    {
        VMRegAnchor         _;
        as_error            error;
        as_key              key;
        char                *digest_p = NULL;

        as_error_init(&error);

        get_digest_from_key(key, ns, set, primary_key, &digest_p, error);
        if (error.code == AEROSPIKE_OK && *digest_p) {
            as_key_destroy(&key);
            String s = String((char*)digest_p, AS_DIGEST_VALUE_SIZE, CopyString);
            free(digest_p);
            return s;
        }

        return String();
    }
    /* }}} */

    /* {{{ proto int Aerospike::register( String path, String module [, int language = Aerospike::UDF_TYPE_LUA [, array options]] )
       Registers a UDF module with the Aerospike cluster */
    int64_t HHVM_METHOD(Aerospike, register, const Variant& path, const Variant& module,
            const Variant& language, const Variant& options)
    {
        VMRegAnchor         _;
        auto                data = Native::data<Aerospike>(this_);
        as_error            error;
        as_policy_info      info_policy;
        PolicyManager       policy_manager;

        as_error_init(&error);

        if (!data->as_ref_p || !data->as_ref_p->as_p) {
            as_error_update(&error, AEROSPIKE_ERR_CLIENT,
                    "Invalid aerospike connection object");
        } else if (!data->is_connected) {
            as_error_update(&error, AEROSPIKE_ERR_CLUSTER,
                    "Register : Connection not established");
        } else {
            if (AEROSPIKE_OK == policy_manager.initPolicyManager(&info_policy,
                        "info", &data->as_ref_p->as_p->config, error) &&
                    AEROSPIKE_OK == policy_manager.set_policy(NULL,
                        data->serializer_value, options, error)) {
                register_udf_module(data->as_ref_p->as_p, path, module, language, &info_policy, error);
            }
        }

        data->setError(error);
        return error.code;
    }
    /* }}} */

    /* {{{ proto int Aerospike::deregister( String module [, array options]] )
       Removes a UDF module from the Aerospike cluster */
    int64_t HHVM_METHOD(Aerospike, deregister, const Variant& module, const Variant& options)
    {
        VMRegAnchor         _;
        auto                data = Native::data<Aerospike>(this_);
        as_error            error;
        as_policy_info      info_policy;
        PolicyManager       policy_manager;

        as_error_init(&error);

        if (!data->as_ref_p || !data->as_ref_p->as_p) {
            as_error_update(&error, AEROSPIKE_ERR_CLIENT,
                    "Invalid aerospike connection object");
        } else if (!data->is_connected) {
            as_error_update(&error, AEROSPIKE_ERR_CLUSTER,
                    "Deregister : Connection not established");
        } else {
            if (AEROSPIKE_OK == policy_manager.initPolicyManager(&info_policy,
                        "info", &data->as_ref_p->as_p->config, error) &&
                    AEROSPIKE_OK == policy_manager.set_policy(NULL,
                        data->serializer_value, options, error)) {
                remove_udf_module(data->as_ref_p->as_p, module, &info_policy, error);
            }
        }

        data->setError(error);
        return error.code;
    }
    /* }}} */

    /* {{{ proto int Aerospike::getRegistered( String module, String &$module_code [, int $language = Aerospike::UDF_TYPE_LUA [, array options]] )
       Gets the code of the registered UDF module */
    int64_t HHVM_METHOD(Aerospike, getRegistered, const Variant& module, VRefParam module_code,
            const Variant& language, const Variant& options)
    {
        VMRegAnchor         _;
        auto                data = Native::data<Aerospike>(this_);
        as_error            error;
        as_policy_info      info_policy;
        PolicyManager       policy_manager;
        String              udf_file_content;

        as_error_init(&error);

        if (!data->as_ref_p || !data->as_ref_p->as_p) {
            as_error_update(&error, AEROSPIKE_ERR_CLIENT,
                    "Invalid aerospike connection object");
        } else if (!data->is_connected) {
            as_error_update(&error, AEROSPIKE_ERR_CLUSTER,
                    "getRegistered : Connection not established");
        } else {
            if (AEROSPIKE_OK == policy_manager.initPolicyManager(&info_policy,
                        "info", &data->as_ref_p->as_p->config, error) &&
                    AEROSPIKE_OK == policy_manager.set_policy(NULL,
                        data->serializer_value, options, error)) {
                get_registered_udf_module_code(data->as_ref_p->as_p, module, udf_file_content, language,
                        &info_policy, error);
                module_code.assignIfRef(udf_file_content);
            }
        }

        data->setError(error);
        return error.code;
    }
    /* }}} */

    /* {{{ proto int Aerospike::listRegistered( array &$modules [, int $language = Aerospike::UDF_TYPE_LUA [, array options]] )
       Lists the UDF modules registered with the server */
    int64_t HHVM_METHOD(Aerospike, listRegistered, VRefParam modules, const Variant& language, const Variant& options)
    {
        VMRegAnchor         _;
        auto                data = Native::data<Aerospike>(this_);
        as_error            error;
        as_policy_info      info_policy;
        PolicyManager       policy_manager;

        as_error_init(&error);

        if (!data->as_ref_p || !data->as_ref_p->as_p) {
            as_error_update(&error, AEROSPIKE_ERR_CLIENT,
                    "Invalid aerospike connection object");
        } else if (!data->is_connected) {
            as_error_update(&error, AEROSPIKE_ERR_CLUSTER,
                    "listRegistered : Connection not established");
        } else {
            if (AEROSPIKE_OK == policy_manager.initPolicyManager(&info_policy,
                        "info", &data->as_ref_p->as_p->config, error) &&
                    AEROSPIKE_OK == policy_manager.set_policy(NULL,
                        data->serializer_value, options, error)) {
                Array temp_modules = Array::Create();
                if (AEROSPIKE_OK == list_registered_udf_modules(data->as_ref_p->as_p, temp_modules, language,
                            &info_policy, error)) {
                    modules.assignIfRef(temp_modules);
                }
            }
        }

        data->setError(error);
        return error.code;
    }

    /* {{{ proto int Aerospike::apply( array key, String module, String function [, array args [, mixed &returned [, array options]] )
       Applies a UDF to a record */
    int64_t HHVM_METHOD(Aerospike, apply, const Array& php_key, const Variant& module, const Variant& function,
            const Variant& args, VRefParam returned_value, const Variant& options)
    {
        VMRegAnchor         _;
        auto                data = Native::data<Aerospike>(this_);
        as_error            error;
        as_key              key;
        as_policy_apply     apply_policy;
        int16_t             serializer_type = SERIALIZER_PHP;
        StaticPoolManager   static_pool;
        bool                key_initialized = false;
        PolicyManager       policy_manager;
        Variant             temp_returned_value;

        as_error_init(&error);

        if (!data->as_ref_p || !data->as_ref_p->as_p) {
            as_error_update(&error, AEROSPIKE_ERR_CLIENT,
                    "Invalid aerospike connection object");
        } else if (!data->is_connected) {
            as_error_update(&error, AEROSPIKE_ERR_CLUSTER,
                    "Deregister : Connection not established");
        } else if (AEROSPIKE_OK == php_key_to_as_key(php_key, key, error)) {
            key_initialized = true;
            if (AEROSPIKE_OK == policy_manager.initPolicyManager(&apply_policy,
                        "apply", &data->as_ref_p->as_p->config, error) &&
                    AEROSPIKE_OK == policy_manager.set_policy(&serializer_type,
                        data->serializer_value, options, error)) {
                aerospike_udf_apply(data->as_ref_p->as_p, key, module, function, args,
                        &apply_policy, static_pool, serializer_type, temp_returned_value, error);
                returned_value.assignIfRef(temp_returned_value);
            }
        }

        if (key_initialized) {
            as_key_destroy(&key);
        }

        data->setError(error);
        return error.code;
    }
    /* }}} */

    /* {{{ proto int Aerospike::scan( string ns, string set, callback record_cb * [, array select [, array options ]] )
       Returns all the records in a set to a callback method  */
    int64_t HHVM_METHOD(Aerospike, scan, const Variant &ns, const Variant &set, const Variant &function,
            const Variant &bins, const Variant &options)
    {
        VMRegAnchor         _;
        auto                data = Native::data<Aerospike>(this_);
        as_error            error;
        as_scan             scan;
        as_policy_scan      scan_policy;
        bool                scan_initialized = false;
        PolicyManager       policy_manager;

        foreach_callback_user_udata      udata(function, error);

        as_error_init(&error);

        if (!data->as_ref_p || !data->as_ref_p->as_p) {
            as_error_update(&error, AEROSPIKE_ERR_CLIENT,
                    "Invalid aerospike connection object");
        } else if (!data->is_connected) {
            as_error_update(&error, AEROSPIKE_ERR_CLUSTER,
                    "scan: connection not established");
        } else if (!function.isObject()) {
            as_error_update(&error, AEROSPIKE_ERR_PARAM,
                    "Parameter 3 must be a function object");
        } else if (AEROSPIKE_OK == initialize_scan(&scan, ns, set, bins, error)) {
            scan_initialized = true;
            if (AEROSPIKE_OK == policy_manager.initPolicyManager(&scan_policy,
                        "scan", &data->as_ref_p->as_p->config, error) &&
                    AEROSPIKE_OK == policy_manager.set_policy(NULL,
                        data->serializer_value, options, error) &&
                    AEROSPIKE_OK == set_scan_policies(&scan, options, error)) {
                aerospike_scan_foreach(data->as_ref_p->as_p, &error,
                        &scan_policy, &scan, scan_query_callback, &udata);
            }
        }

        if (scan_initialized) {
            as_scan_destroy(&scan);
        }

        data->setError(error);
        return error.code;
    }
    /* }}} */

    /* {{{ proto int Aerospike::scanApply( string ns, string set, string module, * string function, array args, int &scan_id [, array options ] )
       Applies a record UDF to each record of a set using a background scan  */
    int64_t HHVM_METHOD(Aerospike, scanApply, const Variant &ns, const Variant &set, const Variant &module,
            const Variant &function, const Variant &args, VRefParam scan_id, const Variant &options)
    {
        VMRegAnchor         _;
        auto                data = Native::data<Aerospike>(this_);
        as_error            error;
        as_scan             scan;
        uint64_t            _scan_id = 0;
        StaticPoolManager   static_pool;
        as_policy_scan      scan_policy;
        as_policy_info      info_policy;
        bool                scan_initialized = false;
        bool                is_wait = true;
        int16_t             serializer_type = SERIALIZER_PHP;
        PolicyManager       policy_manager_scan;
        PolicyManager       policy_manager_info;

        as_error_init(&error);

        if (!data->as_ref_p || !data->as_ref_p->as_p) {
            as_error_update(&error, AEROSPIKE_ERR_CLIENT,
                    "Invalid aerospike connection object");
        } else if (!data->is_connected) {
            as_error_update(&error, AEROSPIKE_ERR_CLUSTER,
                    "scanApply: connection not established");
        /*} else if (scan_id.isNull()) {
            as_error_update(&error, AEROSPIKE_ERR_PARAM,
                    "scan_id argument is invalid");*/
        } else if ((AEROSPIKE_OK == policy_manager_scan.initPolicyManager(&scan_policy,
                        "scan", &data->as_ref_p->as_p->config, error) &&
                    AEROSPIKE_OK == policy_manager_scan.set_policy(&serializer_type,
                        data->serializer_value, options, error)) &&
                (AEROSPIKE_OK == initialize_scanApply(&scan, ns, set, module,
                                                      function, args, static_pool,
                                                      serializer_type, error))) {
            scan_initialized = true;
            if (AEROSPIKE_OK == set_scan_policies(&scan, options, error)) {
                if (AEROSPIKE_OK == aerospike_scan_background(data->as_ref_p->as_p, &error,
                            &scan_policy, &scan, &_scan_id)) {
                    scan_id.assignIfRef((int64_t)_scan_id);
                    //Background scan started, now we can check the status
                    if (is_wait && AEROSPIKE_OK == policy_manager_info.initPolicyManager(&info_policy,
                                "info", &data->as_ref_p->as_p->config, error) &&
                            AEROSPIKE_OK == policy_manager_scan.set_policy(NULL,
                                data->serializer_value, options, error)) {
                        aerospike_scan_wait(data->as_ref_p->as_p, &error, &info_policy, _scan_id, 0);
                    }
                }
            }
        }

        if (scan_initialized) {
            as_scan_destroy(&scan);
        }

        data->setError(error);
        return error.code;
    }
    /* }}} */

    /* {{{ proto int Aerospike::scanInfo ( int scan_id, array &info [, array $options ] )
       Gets the status of a background scan triggered by scanApply()  */
    int64_t HHVM_METHOD(Aerospike, scanInfo, const Variant &scan_id, VRefParam scan_info, const Variant &options)
    {
        VMRegAnchor         _;
        auto                data = Native::data<Aerospike>(this_);
        as_error            error;
        as_scan_info        _scan_info;
        uint64_t            _scan_id = 0;
        as_policy_info      info_policy;
        PolicyManager       policy_manager;

        as_error_init(&error);

        if (!data->as_ref_p || !data->as_ref_p->as_p) {
            as_error_update(&error, AEROSPIKE_ERR_CLIENT,
                    "Invalid aerospike connection object");
        } else if (!data->is_connected) {
            as_error_update(&error, AEROSPIKE_ERR_CLUSTER,
                    "scanInfo: connection not established");
        } else if (!scan_id.isNull() && !scan_id.isInteger()) {
            as_error_update(&error, AEROSPIKE_ERR_PARAM,
                    "scan_id argument is invalid");
        } else if (AEROSPIKE_OK == policy_manager.initPolicyManager(&info_policy,
                    "info", &data->as_ref_p->as_p->config, error) &&
                AEROSPIKE_OK == policy_manager.set_policy(NULL,
                    data->serializer_value, options, error)) {
            if (!scan_id.isNull()) {
                _scan_id = scan_id.toInt64();
            }
            if (AEROSPIKE_OK == aerospike_scan_info(data->as_ref_p->as_p, &error,
                        &info_policy, _scan_id, &_scan_info)) {
                Array php_scan_info = Array::Create();
                php_scan_info.set(s_progress_pct, (int64_t)_scan_info.progress_pct);
                php_scan_info.set(s_records_scanned, (int64_t)_scan_info.records_scanned);
                php_scan_info.set(s_status, (int64_t)_scan_info.status);
                scan_info.assignIfRef(php_scan_info);
            }
        }

        data->setError(error);
        return error.code;
    }
    /* }}} */

    /* {{{ proto array Aerospike::predicateEquals( string bin, int|string val )
       Helper which builds the 'WHERE EQUALS' predicate */
    Variant HHVM_STATIC_METHOD(Aerospike, predicateEquals, const Variant &bin, const Variant &value)
    {
        Array       where = Array::Create();
        bool        isNull = false;

        isNull = construct_Equals_Contains_Predicates(where, bin, value);

        if (isNull) {
            return init_null_variant;
        } else {
            return where;
        }
    }
    /* }}} */

    /* {{{ proto array Aerospike::predicateContains( string bin, int index_type, int|string val )
       Helper which builds the 'WHERE CONTAINS' predicate */
    Variant HHVM_STATIC_METHOD(Aerospike, predicateContains, const Variant &bin, const Variant &index_type, const Variant &value)
    {
        Array       where = Array::Create();
        bool        isNull = false;

        if (!index_type.isNull() && !index_type.isInteger()) {
            //Index type must be integer
            isNull = true;
        } else {
            isNull = construct_Equals_Contains_Predicates(where, bin, value, index_type.toInt64(), true);
        }

        if (isNull) {
            return init_null_variant;
        } else {
            return where;
        }
    }
    /* }}} */

    /* {{{ proto array Aerospike::predicateBetween( string bin, int min, int max )
       Helper which builds the 'WHERE BETWEEN' predicate */
    Variant HHVM_STATIC_METHOD(Aerospike, predicateBetween, const Variant &bin, const Variant &min, const Variant &max)
    {
        Array       where = Array::Create();
        bool        isNull = false;

        isNull = construct_Between_Range_Predicates(where, bin, min, max);

        if (isNull) {
            return init_null_variant;
        } else {
            return where;
        }
    }
    /* }}} */

    /* {{{ proto array Aerospike::predicateRange( string bin, int index_type, int|string min, int|string max )
       Helper which builds the 'WHERE RANGE' predicate */
    Variant HHVM_STATIC_METHOD(Aerospike, predicateRange, const Variant &bin, const Variant &index_type,
            const Variant &min, const Variant &max)
    {
        Array       where = Array::Create();
        bool        isNull = false;

        if (!index_type.isNull() && !index_type.isInteger()) {
            //Index type must be integer
            isNull = true;
        } else {
            isNull = construct_Between_Range_Predicates(where, bin, min, max, index_type.toInt64(), true);
        }

        if (isNull) {
            return init_null_variant;
        } else {
            return where;
        }
    }
    /* }}} */

    /* {{{ proto int Aerospike::query( string ns, string set, array where, callback record_cb [, array select [, array options ]] )
       Queries a secondary index on a set for records matching the where predicate  */
    int64_t HHVM_METHOD(Aerospike, query, const Variant &ns, const Variant &set, const Variant &where,
            const Variant &function, const Variant &bins, const Variant &options)
    {
        VMRegAnchor         _;
        auto                data = Native::data<Aerospike>(this_);
        as_error            error;
        as_query            query;
        as_policy_query     query_policy;
        bool                query_initialized = false;
        PolicyManager       policy_manager;

        foreach_callback_user_udata      udata(function, error);

        as_error_init(&error);

        if (!data->as_ref_p || !data->as_ref_p->as_p) {
            as_error_update(&error, AEROSPIKE_ERR_CLIENT,
                    "Invalid aerospike connection object");
        } else if (!data->is_connected) {
            as_error_update(&error, AEROSPIKE_ERR_CLUSTER,
                    "query: connection not established");
        } else if (!function.isObject()) {
            as_error_update(&error, AEROSPIKE_ERR_PARAM,
                    "Parameter 4 must be a function object");
        } else if (AEROSPIKE_OK == initialize_query(&query, ns, set, where, bins, error)) {
            query_initialized = true;
            if (AEROSPIKE_OK == policy_manager.initPolicyManager(&query_policy,
                        "query", &data->as_ref_p->as_p->config, error) &&
                    AEROSPIKE_OK == policy_manager.set_policy(NULL,
                        data->serializer_value, options, error)) {
                aerospike_query_foreach(data->as_ref_p->as_p, &error,
                        &query_policy, &query, scan_query_callback, &udata);
            }
        }

        if (query_initialized) {
            as_query_destroy(&query);
        }

        data->setError(error);
        return error.code;
    }
    /* }}} */

    /* {{{ proto int Aerospike::aggregate( string ns, string set, array where, string module, string function, array args, mixed &returned [, array options ] )
       Applies a stream UDF to the records matching a query and aggregates the results  */
    int64_t HHVM_METHOD(Aerospike, aggregate, const Variant &ns, const Variant &set, const Variant &where,
            const Variant &module, const Variant &function, const Variant &args, VRefParam result, const Variant &options)
    {
        VMRegAnchor         _;
        auto                data = Native::data<Aerospike>(this_);
        as_error            error;
        as_query            query;
        as_policy_query     query_policy;
        bool                query_initialized = false;
        StaticPoolManager   static_pool;
        int16_t             serializer_type = SERIALIZER_PHP;
        Array               aggregate_array = Array::Create();
        Variant             result_variant;
        PolicyManager       policy_manager;

        foreach_callback_udata      udata(aggregate_array, error);

        as_error_init(&error);

        if (!data->as_ref_p || !data->as_ref_p->as_p) {
            as_error_update(&error, AEROSPIKE_ERR_CLIENT,
                    "Invalid aerospike connection object");
        } else if (!data->is_connected) {
            as_error_update(&error, AEROSPIKE_ERR_CLUSTER,
                    "aggregate: connection not established");
        } else if (AEROSPIKE_OK == initialize_aggregate(&query, ns, set, where, module,
                    function, args, static_pool, serializer_type, error)) {
            query_initialized = true;
            if (AEROSPIKE_OK == policy_manager.initPolicyManager(&query_policy,
                        "query", &data->as_ref_p->as_p->config, error) &&
                    AEROSPIKE_OK == policy_manager.set_policy(NULL,
                        data->serializer_value, options, error)) {
                aerospike_query_foreach(data->as_ref_p->as_p, &error,
                        &query_policy, &query, aggregate_callback, &udata);
                //Copy the returne data of aggregate in out variable
                result.assignIfRef(aggregate_array);
            }
        }

        if (query_initialized) {
            as_query_destroy(&query);
        }

        data->setError(error);

        result_variant = aggregate_array;
        result_variant.releaseForSweep();

        return error.code;
    }
    /* }}} */

    /* {{{ proto string Aerospike::error ( void )
       Displays the error message associated with the last operation */
    int64_t HHVM_METHOD(Aerospike, errorno)
    {
        return Native::data<Aerospike>(this_)->getError().code;
    }
    /* }}} */

    /* {{{ proto int Aerospike::errorno ( void )
       Displays the status code associated with the last operation */
    String HHVM_METHOD(Aerospike, error)
    {
        return Native::data<Aerospike>(this_)->getError().message;
    }
    /* }}} */

    /*
     ************************************************************************************
     * AerospikeExtension class extends HPHP::Extension class and provides the
     * API for PHP userland.
     ************************************************************************************
     */

    class AerospikeExtension : public Extension {
        public:
            AerospikeExtension(): Extension("aerospike", "1.0") {}
            void moduleInit() override
            {
                for (uint32_t i = 0; i <= EXTENSION_CONSTANTS_SIZE; i++) {
                    Native::registerClassConstant<KindOfInt64>(s_Aerospike.get(),
                            StaticString(extension_constants[i].constant_name).get(),
                            extension_constants[i].constant_no);
                }
                HHVM_ME(Aerospike, __construct);
                HHVM_ME(Aerospike, isConnected);
                HHVM_ME(Aerospike, close);
                HHVM_ME(Aerospike, reconnect);
                HHVM_ME(Aerospike, put);
                HHVM_ME(Aerospike, get);
                HHVM_ME(Aerospike, getMany);
                HHVM_ME(Aerospike, addIndex);
                HHVM_ME(Aerospike, dropIndex);
                HHVM_ME(Aerospike, operate);
                HHVM_ME(Aerospike, remove);
                HHVM_ME(Aerospike, removeBin);
                HHVM_ME(Aerospike, exists);
                HHVM_ME(Aerospike, existsMany);
                HHVM_ME(Aerospike, getKeyDigest);
                HHVM_ME(Aerospike, register);
                HHVM_ME(Aerospike, deregister);
                HHVM_ME(Aerospike, getRegistered);
                HHVM_ME(Aerospike, listRegistered);
                HHVM_ME(Aerospike, apply);
                HHVM_ME(Aerospike, scan);
                HHVM_ME(Aerospike, scanApply);
                HHVM_ME(Aerospike, scanInfo);
                HHVM_STATIC_ME(Aerospike, predicateEquals);
                HHVM_STATIC_ME(Aerospike, predicateContains);
                HHVM_STATIC_ME(Aerospike, predicateBetween);
                HHVM_STATIC_ME(Aerospike, predicateRange);
                HHVM_ME(Aerospike, query);
                HHVM_ME(Aerospike, aggregate);
                HHVM_ME(Aerospike, errorno);
                HHVM_ME(Aerospike, error);
                HHVM_STATIC_ME(Aerospike, setSerializer);
                HHVM_STATIC_ME(Aerospike, setDeserializer);
                Native::registerNativeDataInfo<Aerospike>(s_Aerospike.get());
                pthread_rwlock_init(&connection_mutex, NULL);
                pthread_rwlock_init(&scan_query_callback_mutex, NULL);

                loadSystemlib();
            }

            void threadInit() override
            {
                IniSetting::Bind(this, IniSetting::PHP_INI_ALL,
                        "aerospike.connect_timeout",
                        "1000", &ini_entry.connect_timeout);
                IniSetting::Bind(this, IniSetting::PHP_INI_ALL,
                        "aerospike.read_timeout",
                        "1000", &ini_entry.read_timeout);
                IniSetting::Bind(this, IniSetting::PHP_INI_ALL,
                        "aerospike.write_timeout",
                        "1000", &ini_entry.write_timeout);
                IniSetting::Bind(this, IniSetting::PHP_INI_ALL,
                        "aerospike.key_policy",
                        "0", &ini_entry.key_policy);
                IniSetting::Bind(this, IniSetting::PHP_INI_ALL,
                        "aerospike.serializer_type",
                        "1", &ini_entry.serializer_type);
                IniSetting::Bind(this, IniSetting::PHP_INI_ALL,
                        "aerospike.nesting_depth",
                        "3", &ini_entry.nesting_depth);
                IniSetting::Bind(this, IniSetting::PHP_INI_ALL,
                        "aerospike.log_path",
                        NULL, &ini_entry.log_path);
                IniSetting::Bind(this, IniSetting::PHP_INI_ALL,
                        "aerospike.log_level",
                        NULL, &ini_entry.log_level);
                IniSetting::Bind(this, IniSetting::PHP_INI_ALL,
                        "aerospike.shm.use",
                        "false", &ini_entry.shm_use);
                IniSetting::Bind(this, IniSetting::PHP_INI_ALL,
                        "aerospike.shm.max_nodes",
                        "16", &ini_entry.shm_max_nodes);
                IniSetting::Bind(this, IniSetting::PHP_INI_ALL,
                        "aerospike.shm.max_namespaces",
                        "8", &ini_entry.shm_max_namespaces);
                IniSetting::Bind(this, IniSetting::PHP_INI_ALL,
                        "aerospike.shm.takeover_threshold_sec",
                        "30", &ini_entry.shm_takeover_threshold_sec);
                IniSetting::Bind(this, IniSetting::PHP_INI_ALL,
                        "aerospike.udf.lua_system_path",
                        "/opt/aerospike/client-php/sys-lua",
                        &ini_entry.lua_system_path);
                IniSetting::Bind(this, IniSetting::PHP_INI_ALL,
                        "aerospike.udf.lua_user_path",
                        "/opt/aerospike/client-php/usr-lua",
                        &ini_entry.lua_user_path);
                IniSetting::Bind(this, IniSetting::PHP_INI_ALL,
                        "aerospike.shm.shm_key",
                        "0xA5000000",
                        &ini_entry.shm_key);
            }

            void moduleShutdown() override
            {
                as_error error;
                aerospike_ref *map_entry = NULL;

                as_error_init(&error);

                pthread_rwlock_wrlock(&connection_mutex);
                auto it = persistent_list.begin();
                while (it != persistent_list.end()) {
                    map_entry = it->second;

                    if (map_entry) {
                        if (map_entry->ref_host_entry > 1) {
                            map_entry->ref_host_entry--;
                        } else {
                            if (map_entry->as_p) {
                                aerospike_close(map_entry->as_p, &error);
                                aerospike_destroy(map_entry->as_p);
                            }
                            map_entry->ref_host_entry = 0;
                            map_entry->as_p = NULL;
                            if (map_entry) {
                                free(map_entry);
                            }
                            map_entry = NULL;
                        }
                    } else {
                        //Invalid aerospike object
                    }
                    ++it;
                }
                persistent_list.erase(persistent_list.begin(), persistent_list.end());
                pthread_rwlock_wrlock(&connection_mutex);
            }
            //free_shm_key();
    } s_aerospike_extension;

    HHVM_GET_MODULE(aerospike);

} // namespace HPHP

