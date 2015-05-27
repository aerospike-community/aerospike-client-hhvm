#include "batch_op_manager.h"
#include "conversions.h"
#include "helper.h"

namespace HPHP {

    /*
     *******************************************************************************************
     * Constructors for BatchOpManager
     *******************************************************************************************
     */
    BatchOpManager::BatchOpManager() {}

    /*
     *******************************************************************************************
     * This constructor initializes the stack-allocated C client's as_batch instance with
     * the no. of keys in the php_keys array passed here.
     *
     * @param php_keys      PHP Array reference to the PHP keys array to be used
     *                      for the batch operation.
     *******************************************************************************************
     */
    BatchOpManager::BatchOpManager(const Array& php_keys)
    {
        uint16_t batch_iter = 0;
        std::exception e;

        as_batch_init(&this->batch, php_keys.size());
        for (ArrayIter iter(php_keys); iter; ++iter) {
            Variant php_key = iter.second();
            if (php_key.isArray()) {
                as_key *key_p = as_batch_keyat(&this->batch, batch_iter);
                as_error error;
                as_error_init(&error);
                if (AEROSPIKE_OK != php_key_to_as_key(php_key.toArray(),
                            *key_p, error)) {
                    throw e;
                }
                batch_iter++;
            } else {
                throw e;
            }
        }
    }

    /*
     *******************************************************************************************
     * Private member function that populates the outer_meta_array
     * (result for getMany/existsMany, an assoc array) with inner_meta_array
     * using the key_p as the key for the assoc array.
     *
     * @param key_p                 as_key pointer, to be used as the key for the
     *                              outer_meta_array.
     * @param outer_meta_array      PHP Array reference to the result (outer metadata array)
     *                              to be populated with the current value
     *                              (inner_meta_array) using the key_p as the
     *                              current key.
     * @param inner_meta_array      PHP Array reference to the inner_meta_array
     *                              which is the current value to be populated
     *                              within the outer_meta_array.
     * @param error                 as_error reference to be populated by this
     *                              method in case of error.
     *******************************************************************************************
     */
    void BatchOpManager::populate_result_for_get_exists_many(as_key *key_p,
            Array& outer_meta_array, Array& inner_meta_array,
            as_error& error)
    {
        as_error_reset(&error);

        if (!(as_val*)(key_p->valuep)) {
            outer_meta_array.set(String((char *) key_p->digest.value),
                    inner_meta_array);
        } else {
            switch (((as_val*)(key_p->valuep))->type) {
                case AS_STRING:
                    outer_meta_array.set(String((char *) key_p->value.string.value),
                            inner_meta_array);
                    break;
                case AS_INTEGER:
                    outer_meta_array.set(key_p->value.integer.value,
                            inner_meta_array);
                    break;
                default:
                    as_error_update(&error, AEROSPIKE_ERR_CLIENT,
                            "Invalid key type");
                    break;
            }
        }
    }

    /*
     *******************************************************************************************
     * Private member function that is registered as the callback
     * for batch exists, invoked by the C client.
     *
     * @param results               as_batch_read pointer that holds the batch results.
     * @param n                     number of keys in the batch results.
     * @param udata                 The userdata passed to this callback, to be
     *                              populated with the PHP equivalents of the batch results.
     * @return true if SUCCESS else false.
     *******************************************************************************************
     */
    bool BatchOpManager::batch_exists_cb(const as_batch_read* results,
            uint32_t n, void* udata)
    {
        foreach_callback_udata *exists_cb_udata = (foreach_callback_udata *) udata;
        uint16_t i = 0;
        as_error_reset(&exists_cb_udata->error);

        for (i = 0; i < n; i++) {
            if (results[i].result != AEROSPIKE_OK &&
                    results[i].result != AEROSPIKE_ERR_RECORD_NOT_FOUND) {
                return false;
            }

            Array metadata;
            if (results[i].result == AEROSPIKE_OK) {
                metadata = Array::Create();
                metadata_to_php_metadata(&results[i].record,
                        metadata, exists_cb_udata->error);
                if (AEROSPIKE_OK != exists_cb_udata->error.code) {
                    return false;
                }
            }
            BatchOpManager::populate_result_for_get_exists_many((as_key *) results[i].key,
                    exists_cb_udata->data, metadata, exists_cb_udata->error);
        }
        if (AEROSPIKE_OK != exists_cb_udata->error.code) {
            return false;
        }
        return true;
    }

    /*
     *******************************************************************************************
     * Private member function that is registered as the callback
     * for batch get, invoked by the C client.
     *
     * @param results               as_batch_read pointer that holds the batch results.
     * @param n                     number of keys in the batch results.
     * @param udata                 The userdata passed to this callback, to be
     *                              populated with the PHP equivalents of the batch results.
     * @return true if SUCCESS else false.
     *******************************************************************************************
     */
    bool BatchOpManager::batch_get_cb(const as_batch_read* results,
            uint32_t n, void* udata)
    {
        foreach_callback_udata *get_cb_udata = (foreach_callback_udata *) udata;
        uint16_t i = 0;
        as_error_reset(&get_cb_udata->error);

        for (i = 0; i < n; i++) {
            if (results[i].result != AEROSPIKE_OK &&
                    results[i].result != AEROSPIKE_ERR_RECORD_NOT_FOUND) {
                return false;
            }

            Array record;
            if (results[i].result == AEROSPIKE_OK) {
                record = Array::Create();
                as_record_to_php_record(&results[i].record,
                        (as_key *) results[i].key, record, NULL,
                        get_cb_udata->error);
                if (AEROSPIKE_OK != get_cb_udata->error.code) {
                    return false;
                }
            }
            BatchOpManager::populate_result_for_get_exists_many((as_key *) results[i].key,
                    get_cb_udata->data, record, get_cb_udata->error);
        }
        if (AEROSPIKE_OK != get_cb_udata->error.code) {
            return false;
        }
        return true;
    }
    
    /*
     *******************************************************************************************
     * Public member function that is used to invoke a batch exists operation.
     *
     * @param as_p                  aerospike pointer for the current batch operation.
     * @param php_metadata          The return php_metadata to be populated by
     *                              collective metadata of this batch exists operation.
     * @param batch_policy          The as_policy_batch to be used for this
     *                              operation.
     * @param error                 as_error reference to be populated by this
     *                              method in case of error.
     *
     * @return AEROSPIKE_OK if SUCCESS. Otherwise AEROSPIKE_ERR_*.
     *******************************************************************************************
     */
    as_status BatchOpManager::execute_batch_exists(aerospike *as_p,
            VRefParam php_metadata, as_policy_batch& batch_policy,
            as_error& error)
    {
        as_error_reset(&error);
        Array temp_metadata = Array::Create();
        foreach_callback_udata udata(temp_metadata, error);
        aerospike_batch_exists(as_p, &error, &batch_policy, &this->batch,
                (aerospike_batch_read_callback) &batch_exists_cb, &udata);
        php_metadata = temp_metadata;
        return error.code;
    }
    
    /*
     *******************************************************************************************
     * Public member function that is used to invoke a batch get operation.
     *
     * @param as_p                  aerospike pointer for the current batch operation.
     * @param php_records           The return php_records to be populated by
     *                              collective records of this batch get operation.
     * @param php_filter_bins       The optional php filter bins array used to
     *                              select specific bins in the batch get.
     * @param batch_policy          The as_policy_batch to be used for this
     *                              operation.
     * @param error                 as_error reference to be populated by this
     *                              method in case of error.
     *
     * @return AEROSPIKE_OK if SUCCESS. Otherwise AEROSPIKE_ERR_*.
     *******************************************************************************************
     */
    as_status BatchOpManager::execute_batch_get(aerospike *as_p,
            VRefParam php_records, const Variant& php_filter_bins,
            as_policy_batch& batch_policy,
            as_error& error)
    {
        as_error_reset(&error);
        Array temp_records = Array::Create();
        foreach_callback_udata udata(temp_records, error);

        if (!php_filter_bins.isNull() && !php_filter_bins.isArray()) {
            return as_error_update(&error, AEROSPIKE_ERR_PARAM,
                    "Invalid filter bins type: Expected an array or NULL");
        }

        if (php_filter_bins.isArray()) {
            uint16_t            total_filter_count = php_filter_bins.toArray().size();
            const char          *filter[total_filter_count];
                    
            if (AEROSPIKE_OK == process_filter_bins(php_filter_bins.toArray(),
                        filter, error)) {
                aerospike_batch_get_bins(as_p, &error, &batch_policy,
                        &this->batch, filter, total_filter_count,
                        (aerospike_batch_read_callback) &batch_get_cb,
                        &udata);
            }
        } else {
            aerospike_batch_get(as_p, &error, &batch_policy, &this->batch,
                    (aerospike_batch_read_callback) &batch_get_cb, &udata);
        }
        php_records = temp_records;
        return error.code;
    }

    /*
     *******************************************************************************************
     * Destructor for BatchOpManager, destroys the maintained as_batch instance
     * by this object.
     *******************************************************************************************
     */
    BatchOpManager::~BatchOpManager()
    {
        as_batch_destroy(&this->batch);
    }
} // namespace HPHP

