#ifndef __BATCH_OP_MANAGER_H__
#define __BATHC_OP_MANAGER_H__

#include "hphp/runtime/ext/extension.h"
#include "hphp/runtime/base/execution-context.h"
#include "hphp/runtime/vm/native-data.h"

extern "C" {
#include "aerospike/aerospike.h"
#include "aerospike/as_batch.h"
#include "aerospike/aerospike_batch.h"
#include "aerospike/aerospike_key.h"
#include "aerospike/as_status.h"
#include "aerospike/as_policy.h"
}

namespace HPHP {
    /*
     ************************************************************************************
     * BatchOpManager class to invoke the following batch operations:
     * 1. batch exists() aka Aerospike::existsMany()
     * 2. batch get() aka Aerospike::getMany()
     * In order to instantiate this class, specify the PHP keys array to be used
     * in the specific batch operation.
     * The constructor stack-allocates, initializes the C client's as_batch structure
     * using the number of keys in the PHP keys array and maintains it.
     * The destructor destroys the maintained C client's as_batch instance.
     ************************************************************************************
     * Methods:
     ************************************************************************************
     * 1. Use execute_batch_exists() to perform a batch exists operation on the
     * keys provided in the constructor; returns the collective metadata of the
     * said records within the VRefParam php_metadata.
     * 1. Use execute_batch_get() to perform a batch get operation on the
     * keys provided in the constructor; returns all the said records within
     * the VRefParam php_records.
     ************************************************************************************
     */
    class BatchOpManager {
        private:
            as_batch batch;
            static void populate_result_for_get_exists_many(as_key *key_p,
                    Array& outer_meta_array, Array& inner_meta_array,
                    as_error& error);
            static bool batch_exists_cb(const as_batch_read* results, uint32_t n, void* udata);
            static bool batch_get_cb(const as_batch_read* results, uint32_t n, void* udata);
        public:
            BatchOpManager();
            ~BatchOpManager();
            BatchOpManager(const Array& php_keys);
            as_status execute_batch_exists(aerospike *as_p, VRefParam php_metadata,
                    as_policy_batch& batch_policy, as_error& error);
            as_status execute_batch_get(aerospike *as_p, VRefParam php_records,
                    const Variant& filter_bins, as_policy_batch& batch_policy,
                    as_error& error);
    };
}
#endif /* end of __BATCH_OP_MANAGER_H__ */
