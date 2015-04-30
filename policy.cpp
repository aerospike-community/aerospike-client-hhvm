#include "policy.h"

namespace HPHP {
    /*
     *******************************************************************************************
     * Constructors for PolicyManager
     *******************************************************************************************
     */

    /*
     *******************************************************************************************
     * This constructor is used by Aerospike::__construct() to store global policy defaults within
     * as_config.
     *******************************************************************************************
     */
    PolicyManager::PolicyManager(as_config *config_p) {
        this->config_p = config_p;
        this->type = "config";
    }

    /*
     *******************************************************************************************
     * This constructor is used by all Aerospike APIs to set method level policies.
     *******************************************************************************************
     */
    PolicyManager::PolicyManager(void *policy_p, char *type, as_config *config_p) {
        this->policy_holder = policy_p;
        this->type = type;
        this->config_p = config_p;
        initialize_policy();
    }

    /*
     *******************************************************************************************
     * Function to initialize the policy with csdk defaults depending on policy type.
     *******************************************************************************************
     */
    void PolicyManager::initialize_policy() {
        if (this->type) {
            if (strcmp("read", this->type) == 0) {
                POLICY_INIT(read);
            } else if (strcmp("write", this->type) == 0) {
                POLICY_INIT(write);
            } else if (strcmp("operate", this->type) == 0) {
                POLICY_INIT(operate);
            } else if (strcmp("remove", this->type) == 0) {
                POLICY_INIT(remove);
            } else if (strcmp("info", this->type) == 0) {
                POLICY_INIT(info);
            } else if (strcmp("scan", this->type) == 0) {
                POLICY_INIT(scan);
            } else if (strcmp("query", this->type) == 0) {
                POLICY_INIT(query);
            } else if (strcmp("apply", this->type) == 0) {
                POLICY_INIT(apply);
            } else if (strcmp("admin", this->type) == 0) {
                POLICY_INIT(admin);
            } else if (strcmp("batch", this->type) == 0) {
                POLICY_INIT(batch);
            } else {
                /* TBD: handle failure */
            }
        } else {
            /* TBD: handle failure */
        }
    }

    /*
     *******************************************************************************************
     * Function to copy INI entries from php.ini to as_config policies.
     *******************************************************************************************
     */
    as_status PolicyManager::copy_INI_entries_to_config(as_error& error) {
        std::string ini_value;
        uint32_t val = 0;

        as_error_reset(&error);

        if (!this->config_p) {
            return as_error_update(&error, AEROSPIKE_ERR_CLIENT,
                    "config is null");
        }

        if (IniSetting::Get("aerospike.connect_timeout", ini_value)) {
            this->config_p->conn_timeout_ms = atoi(ini_value.c_str());
        }

        if (IniSetting::Get("aerospike.write_timeout", ini_value)) {
            val = atoi(ini_value.c_str());
            this->config_p->policies.write.timeout = val;
            this->config_p->policies.operate.timeout = val;
            this->config_p->policies.remove.timeout = val;
            this->config_p->policies.apply.timeout = val;
        }

        if (IniSetting::Get("aerospike.read_timeout", ini_value)) {
            val = atoi(ini_value.c_str());
            this->config_p->policies.read.timeout = val;
            this->config_p->policies.info.timeout = val;
            this->config_p->policies.batch.timeout = val;
            this->config_p->policies.scan.timeout = val;
            this->config_p->policies.query.timeout = val;
        }

        if (IniSetting::Get("aerospike.key_policy", ini_value)) {
            val = atoi(ini_value.c_str());
            this->config_p->policies.read.key = (as_policy_key) val;
            this->config_p->policies.write.key =  (as_policy_key) val;
            this->config_p->policies.operate.timeout = (as_policy_key) val;
            this->config_p->policies.remove.timeout = (as_policy_key) val;
        }

        return error.code;
    }

    /*
     *******************************************************************************************
     * Function for setting the generation value inside the
     * as_record/as_operations parameter.
     *
     * @param as_record_p       The csdk's as_record.
     * @param options_variant   The user's optional policy options to be used if
     *                          set
     * @param error             as_error reference to be populated by this function
     *                          in case of error
     *
     * @return AEROSPIKE_OK if success. Otherwise AEROSPIKE_ERR_*.
     *******************************************************************************************
     */
    as_status PolicyManager::set_generation_value(uint16_t *gen_value_p, const Variant& options_variant,
            as_error& error) {
        as_error_reset(&error);

        if (!gen_value_p) {
            return as_error_update(&error, AEROSPIKE_ERR_CLIENT,
                    "gen is null");
        }

        Array  options = options_variant.toArray();
        if (options.exists(OPT_POLICY_GEN) && options[OPT_POLICY_GEN].isArray()) {
            Array gen_policy = options[OPT_POLICY_GEN].toArray();
            if ( gen_policy.length() == 2 && gen_policy[1].isInteger()) {
                *gen_value_p = gen_policy[1].toInt32();
            } else {
                return as_error_update(&error, AEROSPIKE_ERR_PARAM,
                        "Invalid generation value type");
            }
        }

        return error.code;
    }

    /*
     *******************************************************************************************
     * Wrapper function for setting the relevant aerospike policies by using the user's
     * optional policy options (if set)
     *
     * @param options_variant   The user's optional policy options to be used if
     *                          set
     * @param error             as_error reference to be populated by this function
     *                          in case of error
     *
     * @return AEROSPIKE_OK if success. Otherwise AEROSPIKE_ERR_*.
     *******************************************************************************************
     */
    as_status PolicyManager::set_policy(const Variant& options_variant, as_error& error) {
        as_error_reset(&error);

        if (!this->type || !this->policy_holder) {
            return as_error_update(&error, AEROSPIKE_ERR_CLIENT,
                    "type/policy holder is null");
        }

        if (!options_variant.isNull() && !options_variant.isArray()) {
            return as_error_update(&error, AEROSPIKE_ERR_CLIENT,
                    "Policy options must be of type an Array");
        }
        
        if (strcmp(this->type, "read") == 0) {
            as_policy_read_copy(&(this->config_p->policies.read), CURRENT_POLICY(read));

            if (options_variant.isArray()) {
                Array  options = options_variant.toArray();
                if (options.exists(OPT_READ_TIMEOUT) && options[OPT_READ_TIMEOUT].isInteger()) {
                    POLICY_SET_FIELD(read, timeout, options[OPT_READ_TIMEOUT].toInt32(), uint32_t);
                }
                if (options.exists(OPT_POLICY_KEY) && options[OPT_POLICY_KEY].isInteger()) {
                    POLICY_SET_FIELD(read, key, options[OPT_POLICY_KEY].toInt32(), as_policy_key);
                }
            }
        } else if (strcmp(this->type, "write") == 0) {
            as_policy_write_copy(&(this->config_p->policies.write), CURRENT_POLICY(write));

            if (options_variant.isArray()) {
                Array  options = options_variant.toArray();
                if (options.exists(OPT_WRITE_TIMEOUT) && options[OPT_WRITE_TIMEOUT].isInteger()) {
                    POLICY_SET_FIELD(write, timeout, options[OPT_WRITE_TIMEOUT].toInt32(), uint32_t);
                }
                if (options.exists(OPT_POLICY_KEY) && options[OPT_POLICY_KEY].isInteger()) {
                    POLICY_SET_FIELD(write, key, options[OPT_POLICY_KEY].toInt32(), as_policy_key);
                }
                if (options.exists(OPT_POLICY_RETRY) && options[OPT_POLICY_RETRY].isInteger()) {
                    POLICY_SET_FIELD(write, retry, options[OPT_POLICY_RETRY].toInt32(), as_policy_retry);
                }
                if (options.exists(OPT_POLICY_GEN) && options[OPT_POLICY_GEN].isArray()) {
                    Array gen_policy = options[OPT_POLICY_GEN].toArray();
                    POLICY_SET_FIELD(write, gen, gen_policy[0].toInt32(), as_policy_gen);
                }
                if (options.exists(OPT_POLICY_EXISTS) && options[OPT_POLICY_EXISTS].isInteger()) {
                    POLICY_SET_FIELD(write, exists, options[OPT_POLICY_EXISTS].toInt32(), as_policy_exists);
                }
            }
        } else if (strcmp(this->type, "operate") == 0) {
            as_policy_operate_copy(&(this->config_p->policies.operate), CURRENT_POLICY(operate));

            if (options_variant.isArray()) {
                Array  options = options_variant.toArray();
                if (options.exists(OPT_WRITE_TIMEOUT) && options[OPT_WRITE_TIMEOUT].isInteger()) {
                    POLICY_SET_FIELD(operate, timeout, options[OPT_WRITE_TIMEOUT].toInt32(), uint32_t);
                }
                if (options.exists(OPT_POLICY_KEY) && options[OPT_POLICY_KEY].isInteger()) {
                    POLICY_SET_FIELD(operate, key, options[OPT_POLICY_KEY].toInt32(), as_policy_key);
                }
                if (options.exists(OPT_POLICY_RETRY) && options[OPT_POLICY_RETRY].isInteger()) {
                    POLICY_SET_FIELD(operate, retry, options[OPT_POLICY_RETRY].toInt32(), as_policy_retry);
                }
                if (options.exists(OPT_POLICY_GEN) && options[OPT_POLICY_GEN].isArray()) {
                    Array gen_policy = options[OPT_POLICY_GEN].toArray();
                    POLICY_SET_FIELD(operate, gen, gen_policy[0].toInt32(), as_policy_gen);
                }
                if (options.exists(OPT_POLICY_REPLICA) && options[OPT_POLICY_REPLICA].isInteger()) {
                    POLICY_SET_FIELD(operate, replica, options[OPT_POLICY_REPLICA].toInt32(), as_policy_replica);
                }
                if (options.exists(OPT_POLICY_CONSISTENCY) && options[OPT_POLICY_CONSISTENCY].isInteger()) {
                    POLICY_SET_FIELD(operate, consistency_level, options[OPT_POLICY_CONSISTENCY].toInt32(), as_policy_consistency_level);
                }
                if (options.exists(OPT_POLICY_COMMIT_LEVEL) && options[OPT_POLICY_COMMIT_LEVEL].isInteger()) {
                    POLICY_SET_FIELD(operate, commit_level, options[OPT_POLICY_COMMIT_LEVEL].toInt32(), as_policy_commit_level);
                }
            }
        } else {
            return as_error_update(&error, AEROSPIKE_ERR_CLIENT,
                    "Invalid type of policy holder");
        }
        
        if (options_variant.isArray()) {
            Array  options = options_variant.toArray();
            if (options.exists(OPT_CONNECT_TIMEOUT) && options[OPT_CONNECT_TIMEOUT].isInteger()) {
                this->config_p->conn_timeout_ms = options[OPT_CONNECT_TIMEOUT].toInt32();
            }
        }

        return error.code;
    }

    /*
     *******************************************************************************************
     * Function for setting the relevant aerospike global defaults by using the user's
     * optional policy options (if set).
     *
     * @param options_variant The user's optional policy options to be used if
     *                        set
     * @param error_p         as_error reference to be populated by this function
     *                        in case of error
     *
     * @return AEROSPIKE_OK if success. Otherwise AEROSPIKE_ERR_*.
     *******************************************************************************************
     */
    as_status PolicyManager::set_config_policies(const Variant& options_variant, as_error& error) {
        as_error_reset(&error);
        copy_INI_entries_to_config(error);

        if (!options_variant.isNull() && !options_variant.isArray()) {
            return as_error_update(&error, AEROSPIKE_ERR_CLIENT,
                    "Policy options must be of type an Array");
        }

        if (options_variant.isArray()) {
            Array  options = options_variant.toArray();
            if (options.exists(OPT_CONNECT_TIMEOUT) && options[OPT_CONNECT_TIMEOUT].isInteger()) {
                this->config_p->conn_timeout_ms = options[OPT_CONNECT_TIMEOUT].toInt32();
            }
            if (options.exists(OPT_READ_TIMEOUT) && options[OPT_READ_TIMEOUT].isInteger()) {
                this->config_p->policies.read.timeout = options[OPT_READ_TIMEOUT].toInt32();
            }
            if (options.exists(OPT_WRITE_TIMEOUT) && options[OPT_WRITE_TIMEOUT].isInteger()) {
                this->config_p->policies.write.timeout = options[OPT_WRITE_TIMEOUT].toInt32();
            }
            if (options.exists(OPT_POLICY_KEY) && options[OPT_POLICY_KEY].isInteger()) {
                this->config_p->policies.key = (as_policy_key) options[OPT_POLICY_KEY].toInt32();
            }
            if (options.exists(OPT_POLICY_RETRY) && options[OPT_POLICY_RETRY].isInteger()) {
                this->config_p->policies.retry = (as_policy_retry) options[OPT_POLICY_RETRY].toInt32();
            }
            if (options.exists(OPT_POLICY_EXISTS) && options[OPT_POLICY_EXISTS].isInteger()) {
                this->config_p->policies.exists = (as_policy_exists) options[OPT_POLICY_EXISTS].toInt32();
            }
            if (options.exists(OPT_POLICY_REPLICA) && options[OPT_POLICY_REPLICA].isInteger()) {
                this->config_p->policies.replica = (as_policy_replica) options[OPT_POLICY_REPLICA].toInt32();
            }   
            if (options.exists(OPT_POLICY_CONSISTENCY) && options[OPT_POLICY_CONSISTENCY].isInteger()) {
                this->config_p->policies.consistency_level = (as_policy_consistency_level) options[OPT_POLICY_CONSISTENCY].toInt32();
            }
            if (options.exists(OPT_POLICY_COMMIT_LEVEL) && options[OPT_POLICY_COMMIT_LEVEL].isInteger()) {
                this->config_p->policies.commit_level = (as_policy_commit_level) options[OPT_POLICY_COMMIT_LEVEL].toInt32();
            }
        }

        return error.code;
    }
} // namespace HPHP
