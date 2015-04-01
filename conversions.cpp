#include "conversions.h"
#include "ext_aerospike.h"

namespace HPHP {
    as_status php_config_to_as_config(Array php_config, as_config *config_p) {
        as_status       status = AEROSPIKE_OK;
        Array           hosts_array = php_config[s_hosts].toArray();
        
        as_config_init(config_p);

        for (int i = 0; i < hosts_array.length(); i++) {
            if (hosts_array[i].isArray()) {
                Array host = hosts_array[i].toArray();
                if (host.exists(s_addr) && host.exists(s_port)) {
                    config_p->hosts[i].addr = host[s_addr].toString().c_str();
                    config_p->hosts[i].port = host[s_port].toInt64();
                } else {
                    status = AEROSPIKE_ERR_PARAM;
                    goto exit;
                }
            } else {
                status = AEROSPIKE_ERR_PARAM;
                goto exit;
            }
        }
exit:
        return status;
    }

    as_status php_key_to_as_key(Array php_key, as_key *key_p) {
        as_status       status = AEROSPIKE_OK;
        const char      *ns_p = NULL;
        const char      *set_p = NULL;

        if (php_key.exists(s_ns) && php_key[s_ns].isString()) {
            ns_p = php_key[s_ns].toString().c_str();
        } else {
            status = AEROSPIKE_ERR_CLIENT;
            goto exit;
        }

        if (php_key.exists(s_set) && php_key[s_set].isString()) {
            set_p = php_key[s_set].toString().c_str();
        } else {
            status = AEROSPIKE_ERR_CLIENT;
            goto exit;
        }

        if (php_key.exists(s_key)) {
            if (php_key[s_key].isInteger()) {
                as_key_init_int64(key_p, ns_p, set_p, php_key[s_key].toInt64());
            } else if (php_key[s_key].isString()) {
                as_key_init_str(key_p, ns_p, set_p, php_key[s_key].toString().c_str());
            } else {
                status = AEROSPIKE_ERR_CLIENT;
                goto exit;
            }
        } else {
            status = AEROSPIKE_ERR_CLIENT;
            goto exit;
        }

exit:
        return status;
    }

    as_status php_record_to_as_record(Array php_rec, as_record *rec_p) {
        as_status       status = AEROSPIKE_OK;
        const char      *bin_name_p = NULL;

        as_record_init(rec_p, php_rec.size());

        for (ArrayIter iter(php_rec); iter; ++iter) {
            Variant key = iter.first();
            if (key.isString()) {
                bin_name_p = key.toString().c_str();
            } else {
                status = AEROSPIKE_ERR_PARAM;
                goto exit;
            }
            Variant value = iter.second();
            if (value.isString()) {
                if (!as_record_set_str(rec_p, bin_name_p, value.toString().c_str())) {
                    status = AEROSPIKE_ERR_PARAM;
                    goto exit;
                }
            } else if (value.isInteger()) {
                if (!as_record_set_int64(rec_p, bin_name_p, value.toInt64())) {
                    status = AEROSPIKE_ERR_PARAM;
                    goto exit;
                }
            }
        }
exit:
        return status;
    }

    bool record_foreach_callback(const char *key, const as_val *value, Array *rec) {
        if (key && value) {
            if (as_val_type(value) == AS_STRING) {
                rec->set(String(key), as_string_get((as_string *) value));
            } else if (as_val_type(value) == AS_INTEGER) {
                rec->set(String(key), as_integer_get((as_integer *) value));
            }
        }
        return true;
    }

    as_status as_record_to_php_record(VRefParam php_rec, as_record *rec_p) {
        as_status       status = AEROSPIKE_OK;
        Array           rec = Array::Create();

        if (!as_record_foreach(rec_p, (as_rec_foreach_callback) record_foreach_callback, &rec)) {
            status = AEROSPIKE_ERR_CLIENT;
            goto exit;
        }
        php_rec = rec;
exit:
        return status;
    }
} // namespace HPHP

