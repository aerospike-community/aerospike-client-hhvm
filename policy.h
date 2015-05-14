#ifndef __POLICY_H__
#define __POLICY_H__

#include "hphp/runtime/ext/extension.h"
#include "hphp/runtime/base/execution-context.h"
#include "hphp/runtime/vm/native-data.h"

extern "C" {
#include "aerospike/aerospike.h"
#include "aerospike/as_config.h"
#include "aerospike/aerospike_key.h"
#include "aerospike/as_status.h"
#include "aerospike/as_policy.h"
#include "constants.h"
}

namespace HPHP {
    /*
     *******************************************************************************************
     * Structure for INI entries
     *******************************************************************************************
     */
    struct ini_entries {
        int64_t connect_timeout;
        int64_t read_timeout;
        int64_t write_timeout;
        int64_t key_policy;
        int64_t nesting_depth;
        std::string log_level;
        std::string log_path;
    };

    extern struct ini_entries ini_entry;

    /*
     ************************************************************************************
     * PolicyManager class to set following policies:
     * 1. Global policy defaults within as_config
     * 2. API level policies
     * In order to instantiate this class, specify the policy pointer
     * (as_policy_X *), type string (for example, "read" for as_policy_read) and
     * as_config *, in case of global defaults in Aerospike constructor.
     ************************************************************************************
     * Methods:
     ************************************************************************************
     * 1. Use set_policy() method to set the appropriate policy fields by parsing the user's
     * options array that consists of policies.
     * 2. Use set_config_policies() method to set the appropriate policy fields
     * in the config's global defaults by parsing the user's options array.
     * 3. Use set_generation_value() method to set the generation value within
     * the passed pointer by parsing the user's options array.
     ************************************************************************************
     */
    class PolicyManager {
        private:
            void *policy_holder;
            char *type;
            as_config *config_p;
            void initialize_policy();
            as_status copy_INI_entries_to_config(as_error& error);
        public:
            PolicyManager()
            {
                this->policy_holder = NULL;
                this->type = NULL;
                this->config_p = NULL;
            }

            PolicyManager(as_config *config_p);
            PolicyManager(void *policy_holder, char *policy_type, as_config *config_p);
            as_status set_policy(const Variant& options, as_error& error);
            as_status set_config_policies(const Variant& options, as_error& error);
            as_status set_generation_value(uint16_t *gen_value, const Variant& options, as_error& error);

/*
 *******************************************************************************************
 MACROS FOR POLICY (Init policy, Get typecasted policy, Set policy fields)
 *******************************************************************************************
 */
            /* Caller of Macro should check for validity of this->policy_holder and this->type */
#define POLICY_INIT(type)                                       \
            if (this->policy_holder) {                          \
                as_policy_##type##_init(CURRENT_POLICY(type));  \
            } else {                                            \
                /*TBD: Handle failure */                        \
            }

            /* Caller of Macro should check for validity of this->policy_holder */
#define CURRENT_POLICY(type) \
            ((as_policy_##type *) (this->policy_holder))

            /* Caller of Macro should check for validity of this->policy_holder and this->type */
#define POLICY_SET_FIELD(type, field, value, typecast) \
            CURRENT_POLICY(type)->field = (typecast) value
    };
}
#endif /* end of __POLICY_H__ */
