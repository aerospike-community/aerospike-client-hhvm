#include "hphp/runtime/base/base-includes.h"
#include "hphp/runtime/base/execution-context.h"
#include "hphp/runtime/vm/native-data.h"
#include "conversions.h"
#include "ext_aerospike.h"

namespace HPHP {

    void HHVM_METHOD(Aerospike, __construct, const Array& php_config) {
        auto                data = Native::data<Aerospike>(this_);
        as_status           status = AEROSPIKE_OK;
        as_error            error;
        as_config           config;

        if (data->as_p) {
            status = AEROSPIKE_ERR_CLIENT;
            /*
             * TODO: Set Aerospike Error Object
             */
            goto exit;
        }

        as_error_init(&error);

        if (php_config.exists(s_hosts) && php_config[s_hosts].isArray()) {
            php_config_to_as_config(php_config, &config);
            data->as_p = aerospike_new(&config);
            status = aerospike_connect(data->as_p, &error);
            if (AEROSPIKE_OK != status) {
                aerospike_destroy(data->as_p);
                data->as_p = NULL;
            }
            data->is_connected = true;
            data->ref_count++;
        } else {
            status = AEROSPIKE_ERR_PARAM;
        }
exit:
        as_error_copy(&data->latest_error, &error);
        return;
    }

    bool HHVM_METHOD(Aerospike, isConnected) {
        auto                data = Native::data<Aerospike>(this_);
        return data->is_connected;
    }

    int HHVM_METHOD(Aerospike, close) {
        auto                data = Native::data<Aerospike>(this_);
        as_error            error;
        as_status           status = AEROSPIKE_OK;

        as_error_init(&error);
        
        data->ref_count--;

        if (0 == data->ref_count) {
            status = aerospike_close(data->as_p, &error);
        }

        data->is_connected = false;
        as_error_copy(&data->latest_error, &error);
        return status;
    }

    int HHVM_METHOD(Aerospike, put, const Array& php_key, const Array& php_rec) {
        auto                data = Native::data<Aerospike>(this_);
        as_error            error;
        as_status           status;
        as_key              key;
        as_record           rec;

        if (!data->as_p) {
            status = AEROSPIKE_ERR_CLIENT;
            goto exit;
        }

        as_error_init(&error);

        if (AEROSPIKE_OK != php_key_to_as_key(php_key, &key)) {
            status = AEROSPIKE_ERR_CLIENT;
            goto exit;
        }

        if (AEROSPIKE_OK != php_record_to_as_record(php_rec, &rec)) {
            status = AEROSPIKE_ERR_CLIENT;
            goto exit;
        }

        status = aerospike_key_put(data->as_p, &error, NULL, &key, &rec);
        as_record_destroy(&rec);
exit:
        as_error_copy(&data->latest_error, &error);
        return status;
    }

    int HHVM_METHOD(Aerospike, get, const Array& php_key, VRefParam php_rec) {
        auto                data = Native::data<Aerospike>(this_);
        as_error            error;
        as_status           status;
        as_key              key;
        as_record           *rec_p = NULL;

        if (!data->as_p) {
            status = AEROSPIKE_ERR_CLIENT;
            goto exit;
        }

        if (AEROSPIKE_OK != (status = php_key_to_as_key(php_key, &key))) {
            goto exit;
        }

        if (AEROSPIKE_OK != (status = aerospike_key_get(data->as_p, &error, NULL, &key, &rec_p))) {
            goto exit;
        }

        if (AEROSPIKE_OK != (status = as_record_to_php_record(php_rec, rec_p))) {
            goto exit;
        }

        as_record_destroy(rec_p);
exit:
        as_error_copy(&data->latest_error, &error);
        return status;
    }

    int HHVM_METHOD(Aerospike, errorno) {
        auto                data = Native::data<Aerospike>(this_);
        return data->latest_error.code;
    }

    String HHVM_METHOD(Aerospike, error) {
        auto                data = Native::data<Aerospike>(this_);
        return data->latest_error.message;
    }

    class AerospikeExtension : public Extension {
        public:
            AerospikeExtension(): Extension("aerospike", "1.0") {}
            void moduleInit() override {
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

