#include "hphp/runtime/base/base-includes.h"
#include "hphp/runtime/base/execution-context.h"
#include "hphp/runtime/vm/native-data.h"
#include "conversions.h"
#include "constants.h"
#include "ext_aerospike.h"

namespace HPHP {

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
    Aerospike::Aerospike() { }

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
    void HHVM_METHOD(Aerospike, __construct, const Array& php_config)
    {
        auto                data = Native::data<Aerospike>(this_);
        as_error            error;
        as_config           config;

        as_error_init(&error);

        if (data->as_p) {
            as_error_update(&error, AEROSPIKE_ERR_CLIENT, "Connection already exists!");
            goto exit;
        }

        if (AEROSPIKE_OK != php_config_to_as_config(php_config, config, error)) {
            goto exit;
        }
        
        data->as_p = aerospike_new(&config);
        if (AEROSPIKE_OK != aerospike_connect(data->as_p, &error)) {
            aerospike_destroy(data->as_p);
            data->as_p = NULL;
            goto exit;
        }
        data->is_connected = true;
        data->ref_count++;
exit:
        as_error_copy(&data->latest_error, &error);
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
            as_error_update(&error, AEROSPIKE_ERR_CLIENT, "Invalid aerospike connection object");
            goto exit;
        }

        if (!data->is_connected ||
                data->ref_count < 1) {
            as_error_update(&error, AEROSPIKE_ERR_CLIENT, "Already disconnected!");
            goto exit;
        }

        data->ref_count--;

        if (0 == data->ref_count) {
            aerospike_close(data->as_p, &error);
        }

        data->is_connected = false;

exit:
        as_error_copy(&data->latest_error, &error);
        return error.code;
    }
    /* }}} */

    /* {{{ proto int Aerospike::put( array key, array record [, int ttl=0 [, array options ]] )
       Writes a record to the cluster */
    int HHVM_METHOD(Aerospike, put, const Array& php_key, const Array& php_rec, int64_t ttl)
    {
        auto                data = Native::data<Aerospike>(this_);
        as_error            error;
        as_key              key;
        as_record           rec;
        StaticPoolManager   static_pool;

        as_error_init(&error);

        if (!data->as_p) {
            as_error_update(&error, AEROSPIKE_ERR_CLIENT, "Invalid aerospike connection object");
            goto exit;
        }

        if (AEROSPIKE_OK != php_key_to_as_key(php_key, key, error)) {
            goto exit;
        }

        if (AEROSPIKE_OK != php_record_to_as_record(php_rec, rec, ttl, static_pool, error)) {
            goto exit;
        }

        aerospike_key_put(data->as_p, &error, NULL, &key, &rec);
        as_record_destroy(&rec);
exit:
        as_error_copy(&data->latest_error, &error);
        return error.code;
    }
    /* }}} */

    /* {{{ proto int Aerospike::get( array key, array record [, array filter [, array options]] )
       Reads a record from the cluster */
    int HHVM_METHOD(Aerospike, get, const Array& php_key, VRefParam php_rec)
    {
        auto                data = Native::data<Aerospike>(this_);
        as_error            error;
        as_key              key;
        as_record           *rec_p = NULL;

        if (!data->as_p) {
            as_error_update(&error, AEROSPIKE_ERR_CLIENT, "Invalid aerospike connection object");
            goto exit;
        }

        if (AEROSPIKE_OK != php_key_to_as_key(php_key, key, error)) {
            goto exit;
        }

        if (AEROSPIKE_OK != aerospike_key_get(data->as_p, &error, NULL, &key, &rec_p)) {
            goto exit;
        }

        if (AEROSPIKE_OK != as_record_to_php_record(rec_p, &key, php_rec, error)) {
            goto exit;
        }

        as_record_destroy(rec_p);
exit:
        as_error_copy(&data->latest_error, &error);
        return error.code;
    }
    /* }}} */

    /* {{{ proto string Aerospike::error ( void )
       Displays the error message associated with the last operation */
    int HHVM_METHOD(Aerospike, errorno)
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
                    Native::registerClassConstant<KindOfInt64>(s_Aerospike.get(), StaticString(extension_constants[i].constant_name).get(), extension_constants[i].constant_no);
                }
                HHVM_ME(Aerospike, __construct);
                HHVM_ME(Aerospike, isConnected);
                HHVM_ME(Aerospike, close);
                HHVM_ME(Aerospike, put);
                HHVM_ME(Aerospike, get);
                HHVM_ME(Aerospike, errorno);
                HHVM_ME(Aerospike, error);
                Native::registerNativeDataInfo<Aerospike>(s_Aerospike.get());
                loadSystemlib();
            }
    } s_aerospike_extension;

    HHVM_GET_MODULE(aerospike);

} // namespace HPHP

