#include "hphp/runtime/ext/extension.h"
#include "hphp/runtime/base/execution-context.h"
#include "hphp/runtime/vm/native-data.h"
#include "conversions.h"
#include "helper.h"
#include "constants.h"
#include "ext_aerospike.h"
#include "policy.h"
#include "batch_op_manager.h"

namespace HPHP {

	ini_entries ini_entry;
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
        pthread_rwlock_init(&connection_mutex, NULL);
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
        if (as_p) {
            if (0 != ref_count) {
                aerospike_close(as_p, &error);
                ref_count = 0;
            }
            aerospike_destroy(as_p);
            as_p = nullptr;
            is_connected = false;
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
     * Definitions of Native methods in PHP Aerospike class declared in
     * ext_aerospike.h and specified in ext_aerospike.php
     ************************************************************************************
     */

    /* {{{ proto Aerospike::__construct(array config [, bool persistent_connection=true [, array options]]))
      Creates a new Aerospike object, with optional persistent connection control */
    void HHVM_METHOD(Aerospike, __construct, const Array& php_config,
            const Variant& options)
    {
        auto                data = Native::data<Aerospike>(this_);
        as_error            error;
        as_config           config;
        PolicyManager       policy_manager(&config);

        as_error_init(&error);

        pthread_rwlock_wrlock(&data->connection_mutex);

        if (data->as_p) {
            as_error_update(&error, AEROSPIKE_ERR_CLIENT,
                    "Connection already exists!");
        } else if (AEROSPIKE_OK == php_config_to_as_config(php_config,
                    config, error)) {
            if (AEROSPIKE_OK == policy_manager.set_config_policies(options,
                        error)) {
                data->as_p = aerospike_new(&config);
                if (AEROSPIKE_OK == aerospike_connect(data->as_p, &error)) {
                    data->is_connected = true;
                    data->ref_count++;
                } else {
                    as_error_update(&error, AEROSPIKE_ERR_CLUSTER,
                            "Unable to connect to server");
                    aerospike_destroy(data->as_p);
                    data->as_p = NULL;
                }
            }
        }

        pthread_rwlock_unlock(&data->connection_mutex);
        pthread_rwlock_wrlock(&data->latest_error_mutex);
        as_error_copy(&data->latest_error, &error);
        pthread_rwlock_unlock(&data->latest_error_mutex);
    }
    /* }}} */

    /* {{{ proto bool Aerospike::isConnected( void )
       Tests whether the connection to the cluster was established */
    bool HHVM_METHOD(Aerospike, isConnected)
    {
        auto                data = Native::data<Aerospike>(this_);

        if (!data->as_p) {
            return false;
        }

        return data->is_connected;
    }
    /* }}} */

    /* {{{ proto Aerospike::close( void )
       Closes all connections to the cluster */
    int HHVM_METHOD(Aerospike, close)
    {
        auto                data = Native::data<Aerospike>(this_);
        as_error            error;

        as_error_init(&error);
        if (!data->as_p) {
            as_error_update(&error, AEROSPIKE_ERR_CLIENT,
                    "Invalid aerospike connection object");
        } else if (!data->is_connected ||
                data->ref_count < 1) {
            as_error_update(&error, AEROSPIKE_ERR_CLIENT,
                    "Already disconnected!");
        } else {
            data->ref_count--;

            if (0 == data->ref_count) {
                aerospike_close(data->as_p, &error);
            }

            data->is_connected = false;
        }

        pthread_rwlock_wrlock(&data->latest_error_mutex);
        as_error_copy(&data->latest_error, &error);
        pthread_rwlock_unlock(&data->latest_error_mutex);
        return error.code;
    }
    /* }}} */

    /* {{{ proto int Aerospike::put( array key, array record [, int ttl=0 [, array options ]] )
       Writes a record to the cluster */
    int64_t HHVM_METHOD(Aerospike, put, const Array& php_key,
            const Array& php_rec, int64_t ttl,
            const Variant& options)
    {
        auto                data = Native::data<Aerospike>(this_);
        as_error            error;
        as_key              key;
        as_record           rec;
        StaticPoolManager   static_pool;
        as_policy_write     write_policy;
        bool                key_initialized = false;
        PolicyManager       policy_manager(&write_policy, "write",
                &data->as_p->config);

        as_error_init(&error);

        if (!data->as_p) {
            as_error_update(&error, AEROSPIKE_ERR_CLIENT,
                    "Invalid aerospike connection object");
        } else if (!data->is_connected) {
            as_error_update(&error, AEROSPIKE_ERR_CLUSTER,
                    "put: connection not established");
        } else if (AEROSPIKE_OK == php_key_to_as_key(php_key, key, error)) {
            key_initialized = true;
            if (AEROSPIKE_OK == php_record_to_as_record(php_rec, rec, ttl,
                        static_pool, error)) {
                if (AEROSPIKE_OK == policy_manager.set_policy(options, error)) {
                    policy_manager.set_generation_value(&rec.gen, options,
                            error);
                    aerospike_key_put(data->as_p, &error, &write_policy,
                            &key, &rec);
                    as_record_destroy(&rec);
                }
            }
        }
        
        if (key_initialized) {
            as_key_destroy(&key);
        }
        pthread_rwlock_wrlock(&data->latest_error_mutex);
        as_error_copy(&data->latest_error, &error);
        pthread_rwlock_unlock(&data->latest_error_mutex);
        return error.code;
    }
    /* }}} */

    /* {{{ proto int Aerospike::get( array key, array record [, array filter [, array options]] )
       Reads a record from the cluster */
    int64_t HHVM_METHOD(Aerospike, get, const Array& php_key, VRefParam php_rec,
            const Variant& filter_bins, const Variant& options)
    {
        auto                data = Native::data<Aerospike>(this_);
        as_status           status = AEROSPIKE_OK;
        as_error            error;
        as_key              key;
        as_record           *rec_p = NULL;
        as_policy_read      read_policy;
        bool                key_initialized = false;
        PolicyManager       policy_manager(&read_policy, "read",
                &data->as_p->config);

        if (!data->as_p) {
            as_error_update(&error, AEROSPIKE_ERR_CLIENT,
                    "Invalid aerospike connection object");
        } else if (!data->is_connected) {
            as_error_update(&error, AEROSPIKE_ERR_CLUSTER,
                    "get: connection not established");
        } else if (AEROSPIKE_OK == php_key_to_as_key(php_key, key, error)) {
            key_initialized = true;
            if (AEROSPIKE_OK == policy_manager.set_policy(options, error)) {
                if (!filter_bins.isNull() && !filter_bins.isArray()) {
                    as_error_update(&error, AEROSPIKE_ERR_CLIENT,
                            "Filter bins must be of type an Array");
                }
                if (filter_bins.isArray()) {
                    status = aerospike_get_filtered_bins(filter_bins.toArray(),
                            data->as_p, &read_policy, key, &rec_p, error);
                } else {
                    status = aerospike_key_get(data->as_p, &error,
                            &read_policy, &key, &rec_p);
                }
                Array temp_php_rec = Array::Create();
                if (status == AEROSPIKE_OK) {
                    as_record_to_php_record(rec_p, &key, temp_php_rec,
                            &read_policy.key, error);
                }
                php_rec = temp_php_rec;
                as_record_destroy(rec_p);
            }
        }
        
        if (key_initialized) {
            as_key_destroy(&key);
        }
        pthread_rwlock_wrlock(&data->latest_error_mutex);
        as_error_copy(&data->latest_error, &error);
        pthread_rwlock_unlock(&data->latest_error_mutex);
        return error.code;
    }
    /* }}} */

    /* {{{ proto int Aerospike::getMany( array keys, array &records [, array filter [, array options ]] )
       Returns a batch of records from the cluster */
    int64_t HHVM_METHOD(Aerospike, getMany, const Array& php_keys,
            VRefParam php_records, const Variant& filter_bins,
            const Variant& options)
    {
        auto                data = Native::data<Aerospike>(this_);
        as_error            error;
        as_policy_batch     batch_policy;
        PolicyManager       policy_manager(&batch_policy, "batch",
                &data->as_p->config);

        as_error_init(&error);

        if (!data->as_p) {
            as_error_update(&error, AEROSPIKE_ERR_CLIENT,
                    "Invalid aerospike connection object");
        } else if (!data->is_connected) {
            as_error_update(&error, AEROSPIKE_ERR_CLUSTER,
                    "getMany: connection not established");
        } else {
            try {
                BatchOpManager batch_op_manager(php_keys);
                if (AEROSPIKE_OK == policy_manager.set_policy(options,
                            error)) {
                    batch_op_manager.execute_batch_get(data->as_p,
                            php_records, filter_bins, batch_policy, error);
                }
            } catch (const std::exception& e) {
                as_error_update(&error, AEROSPIKE_ERR_CLIENT,
                        "Failed to initialize batch operation");
            }
        }

        pthread_rwlock_wrlock(&data->latest_error_mutex);
        as_error_copy(&data->latest_error, &error);
        pthread_rwlock_unlock(&data->latest_error_mutex);
        return error.code;
    }
    /* }}} */

    /* {{{ proto int Aerospike::operate ( array key, array operations [, array &returned [, array options ]] )
       Performs multiple operations on a record */
    int64_t HHVM_METHOD(Aerospike, operate, const Array& php_key,
            const Array& php_operations, VRefParam returned,
            const Variant& options)
    {
        auto                data = Native::data<Aerospike>(this_);
        as_error            error;
        as_key              key;
        StaticPoolManager   static_pool;
        as_operations       operations;
        as_record           *rec_p = NULL;
        as_policy_operate   operate_policy;
        bool                key_initialized = false;
        PolicyManager       policy_manager(&operate_policy, "operate",
                &data->as_p->config);

        as_error_init(&error);

        if (!data->as_p) {
            as_error_update(&error, AEROSPIKE_ERR_CLIENT,
                    "Invalid aerospike connection object");
        } else if (!data->is_connected) {
            as_error_update(&error, AEROSPIKE_ERR_CLUSTER,
                    "operate: connection not established");
        } else if (AEROSPIKE_OK == php_key_to_as_key(php_key, key, error)) {
            key_initialized = true;
            if (AEROSPIKE_OK == php_operations_to_as_operations(php_operations,
                        operations, static_pool, error)) {
                if (AEROSPIKE_OK == policy_manager.set_policy(options, error)) {
                    policy_manager.set_generation_value(&operations.gen,
                            options, error);
                    as_record_init(rec_p, 0);
                    aerospike_key_operate(data->as_p, &error, &operate_policy,
                            &key, &operations, &rec_p);
                    Array php_rec = Array::Create();
                    if (rec_p) {
                        bins_to_php_bins(rec_p, php_rec, error);
                        as_record_destroy(rec_p);
                    }
                    returned = php_rec;
                }
            }
        }

        if (key_initialized) {
            as_key_destroy(&key);
        }
        pthread_rwlock_wrlock(&data->latest_error_mutex);
        as_error_copy(&data->latest_error, &error);
        pthread_rwlock_unlock(&data->latest_error_mutex);
        return error.code;
    }
    /* }}} */

    /* {{{ proto int Aerospike::remove( array key [, array options ] )
       Removes a record from the cluster */
    int64_t HHVM_METHOD(Aerospike, remove, const Array& php_key,
            const Variant& options)
    {
        auto                data = Native::data<Aerospike>(this_);
        as_error            error;
        as_key              key;
        as_policy_remove    remove_policy;
        bool                key_initialized = false;
        PolicyManager       policy_manager(&remove_policy, "remove",
                &data->as_p->config);

        as_error_init(&error);

        if (!data->as_p) {
            as_error_update(&error, AEROSPIKE_ERR_CLIENT,
                    "Invalid aerospike connection object");
        } else if (!data->is_connected) {
            as_error_update(&error, AEROSPIKE_ERR_CLUSTER,
                    "remove: connection not established");
        } else if (AEROSPIKE_OK == php_key_to_as_key(php_key, key, error)) {
            key_initialized = true;
            if (AEROSPIKE_OK == policy_manager.set_policy(options, error)) {
                policy_manager.set_generation_value(&remove_policy.generation,
                        options, error);
                aerospike_key_remove(data->as_p, &error, &remove_policy, &key);
            }
        }

        if (key_initialized) {
            as_key_destroy(&key);
        }
        pthread_rwlock_wrlock(&data->latest_error_mutex);
        as_error_copy(&data->latest_error, &error);
        pthread_rwlock_unlock(&data->latest_error_mutex);
        return error.code;
    }
    /* }}} */

    /* {{{ proto int Aerospike::removeBin( array key, array bins [, array options ])
       Removes a bin from a record */
    int64_t HHVM_METHOD(Aerospike, removeBin, const Array& php_key,
            const Array& bins, const Variant& options)
    {
        auto                data = Native::data<Aerospike>(this_);
        as_error            error;
        as_key              key;
        as_record           record;
        as_policy_write     write_policy;
        bool                key_initialized = false;
        PolicyManager       policy_manager(&write_policy, "write",
                &data->as_p->config);

        as_error_init(&error);

        if (!data->as_p) {
            as_error_update(&error, AEROSPIKE_ERR_CLIENT,
                    "Invalid aerospike connection object");
        } else if (!data->is_connected) {
            as_error_update(&error, AEROSPIKE_ERR_CLUSTER,
                    "removeBin: connection not established");
        } else if (AEROSPIKE_OK == php_key_to_as_key(php_key, key, error)) {
            key_initialized = true;
            if (AEROSPIKE_OK == policy_manager.set_policy(options, error)) {
                as_record_inita(&record, bins.size());
                policy_manager.set_generation_value(&record.gen,
                        options, error);
                if (AEROSPIKE_OK == set_nil_bins(&record, bins, error)) {
                    aerospike_key_put(data->as_p, &error, &write_policy, &key, &record);
                }
                as_record_destroy(&record);
            }
        }

        if (key_initialized) {
            as_key_destroy(&key);
        }
        pthread_rwlock_wrlock(&data->latest_error_mutex);
        as_error_copy(&data->latest_error, &error);
        pthread_rwlock_unlock(&data->latest_error_mutex);
        return error.code;
    }
    /* }}} */

    /* {{{ proto int Aerospike::exists( array key, array &metadata [, array options] )
       Returns a record's metadata */
    int64_t HHVM_METHOD(Aerospike, exists, const Array& php_key,
            VRefParam metadata, const Variant& options)
    {
        auto                data = Native::data<Aerospike>(this_);
        as_error            error;
        as_key              key;
        as_record           *record_p = NULL;
        as_policy_read      read_policy;
        bool                key_initialized = false;
        PolicyManager       policy_manager(&read_policy, "read",
                &data->as_p->config);

        as_error_init(&error);

        if (!data->as_p) {
            as_error_update(&error, AEROSPIKE_ERR_CLIENT,
                    "Invalid aerospike connection object");
        } else if (!data->is_connected) {
            as_error_update(&error, AEROSPIKE_ERR_CLUSTER,
                    "exists: connection not established");
        } else if (AEROSPIKE_OK == php_key_to_as_key(php_key, key, error)) {
            key_initialized = true;
            if (AEROSPIKE_OK == policy_manager.set_policy(options, error)) {
                if (AEROSPIKE_OK == aerospike_key_exists(data->as_p, &error,
                            &read_policy, &key, &record_p)) {
                    Array php_metadata = Array::Create();
                    metadata_to_php_metadata(record_p, php_metadata, error);
                    metadata = php_metadata;
                }
                as_record_destroy(record_p);
            }
        }

        if (key_initialized) {
            as_key_destroy(&key);
        }
        pthread_rwlock_wrlock(&data->latest_error_mutex);
        as_error_copy(&data->latest_error, &error);
        pthread_rwlock_unlock(&data->latest_error_mutex);
        return error.code;
    }
    /* }}} */

    /* {{{ proto int Aerospike::existsMany( array keys, array &metadata [, array options] )
       Returns metadata for a batch of records with NULL for non-existent ones */
    int64_t HHVM_METHOD(Aerospike, existsMany, const Array& php_keys,
            VRefParam metadata, const Variant& options)
    {
        auto                data = Native::data<Aerospike>(this_);
        as_error            error;
        as_policy_batch     batch_policy;
        PolicyManager       policy_manager(&batch_policy, "batch",
                &data->as_p->config);

        as_error_init(&error);

        if (!data->as_p) {
            as_error_update(&error, AEROSPIKE_ERR_CLIENT,
                    "Invalid aerospike connection object");
        } else if (!data->is_connected) {
            as_error_update(&error, AEROSPIKE_ERR_CLUSTER,
                    "existsMany: connection not established");
        } else {
            try {
                BatchOpManager batch_op_manager(php_keys);
                if (AEROSPIKE_OK == policy_manager.set_policy(options,
                            error)) {
                    batch_op_manager.execute_batch_exists(data->as_p,
                            metadata, batch_policy, error);
                }
            } catch (const std::exception& e) {
                as_error_update(&error, AEROSPIKE_ERR_CLIENT,
                        "Failed to initialize batch operation");
            }
        }

        pthread_rwlock_wrlock(&data->latest_error_mutex);
        as_error_copy(&data->latest_error, &error);
        pthread_rwlock_unlock(&data->latest_error_mutex);
        return error.code;
    }
    /* }}} */

    /* {{{ proto string Aerospike::getKeyDigest( string ns, string set, int|string pk )
       Helper which computes the digest that for a given key */
    String HHVM_METHOD(Aerospike, getKeyDigest, const Variant& ns,
            const Variant& set, const Variant& primary_key)
    {
        as_error            error;
        as_key              key;
        char                *digest_p = NULL;

        as_error_init(&error);

        Array php_key = Array::Create();
        php_key.set(s_ns, ns.toString());
        php_key.set(s_set, set.toString());
        php_key.set(s_key, primary_key);

        get_digest_from_php_key(php_key, key, &digest_p, error);

        if (error.code == AEROSPIKE_OK) {
            as_key_destroy(&key);
        }
        return String(digest_p);
    }
    /* }}} */

    /* {{{ proto string Aerospike::error ( void )
       Displays the error message associated with the last operation */
    int64_t HHVM_METHOD(Aerospike, errorno)
    {
        auto                data = Native::data<Aerospike>(this_);
        return data->latest_error.code;
    }
    /* }}} */

    /* {{{ proto int Aerospike::errorno ( void )
       Displays the status code associated with the last operation */
    String HHVM_METHOD(Aerospike, error)
    {
        auto                data = Native::data<Aerospike>(this_);
        return data->latest_error.message;
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
                HHVM_ME(Aerospike, put);
                HHVM_ME(Aerospike, get);
                HHVM_ME(Aerospike, getMany);
                HHVM_ME(Aerospike, operate);
                HHVM_ME(Aerospike, remove);
                HHVM_ME(Aerospike, removeBin);
                HHVM_ME(Aerospike, exists);
                HHVM_ME(Aerospike, existsMany);
                HHVM_ME(Aerospike, getKeyDigest);
                HHVM_ME(Aerospike, errorno);
                HHVM_ME(Aerospike, error);
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
                        "aerospike.nesting_depth",
                        "3", &ini_entry.nesting_depth);
                IniSetting::Bind(this, IniSetting::PHP_INI_ALL,
                        "aerospike.log_path",
                        NULL, &ini_entry.log_path);
                IniSetting::Bind(this, IniSetting::PHP_INI_ALL,
                        "aerospike.log_level",
                        NULL, &ini_entry.log_level);
                Native::registerNativeDataInfo<Aerospike>(s_Aerospike.get());
                loadSystemlib();
            }
    } s_aerospike_extension;

    HHVM_GET_MODULE(aerospike);

} // namespace HPHP

