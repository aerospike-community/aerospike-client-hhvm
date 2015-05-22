#include "helper.h"
#include "conversions.h"

namespace HPHP {
    /*
     **********************************************************************************************
     * Helper function to generate digest from php key.
     *
     * @param php_key           PHP Array reference to the php_key to be
     *                          converted.
     * @param key               as_key reference to be populated by the
     *                          corresponding as_key for the php_key.
     * @param digest_pp         The return digest string to be populated by this
     *                          function.
     * @param error             The as_error reference to be populated by this
     *                          function in case of error.
     *
     * @return AEROSPIKE_OK if SUCCESS. Otherwise AEROSPIKE_ERR_*.
     **********************************************************************************************
     */
    as_status get_digest_from_php_key(const Array& php_key, as_key& key, char **digest_pp, as_error& error)
    {
        as_error_reset(&error);

        if (AEROSPIKE_OK == php_key_to_as_key(php_key, key, error)) {
            if (as_key_digest(&key) && key.digest.init) {
                *digest_pp = (char *) key.digest.value;
            } else {
                *digest_pp = NULL;
            }
        }
        return error.code;
    }

    /*
     **********************************************************************************************
     * Helper function to set bins of a record to nil.
     *
     * @param record_p          as_record pointer, to the record, the bins of
     *                          which is to be set to as_nil.
     * @param php_bins          PHP Array reference to the php_bins to be set
     *                          as_nil.
     * @param error             The as_error reference to be populated by this
     *                          function in case of error.
     *
     * @return AEROSPIKE_OK if SUCCESS. Otherwise AEROSPIKE_ERR_*.
     **********************************************************************************************
     */
    as_status set_nil_bins(as_record *record_p, const Array& php_bins,
            as_error& error)
    {
        as_error_reset(&error);
        for (ArrayIter iter(php_bins); iter; ++iter) {
            Variant bin = iter.second();
            if (!bin.isString()) {
                return as_error_update(&error, AEROSPIKE_ERR_PARAM,
                        "Bin name must be a string");
            }
            if (!(as_record_set_nil(record_p, bin.toString().c_str()))) {
                return as_error_update(&error, AEROSPIKE_ERR_CLIENT,
                        "Failed to set nil bin in as_record");
            }
        }
        return error.code;
    }

    /*
     **********************************************************************************************
     * Helper function to get filtered/select bins from server.
     *
     * @param php_filter_bins   PHP Array reference to the php_filter_bins to be
     *                          selected by the get/getMany operation.
     * @param filter            The array of char*[] to be populated by this
     *                          function using the bin names (strings) within
     *                          the php_filter_bins.
     * @param error             The as_error reference to be populated by this
     *                          function in case of error.
     *
     * @return AEROSPIKE_OK if SUCCESS. Otherwise AEROSPIKE_ERR_*.
     **********************************************************************************************
     */
    as_status process_filter_bins(const Array& php_filter_bins,
            const char **filter, as_error& error)
    {
        uint16_t            filter_count = 0;

        as_error_reset(&error);
        for (ArrayIter iter(php_filter_bins); iter; ++iter) {
            Variant bin = iter.second();
            if (!bin.isString()) {
                return as_error_update(&error, AEROSPIKE_ERR_PARAM,
                        "Bin name in filter bins must be a string");
            } else {
                filter[filter_count++] = bin.toString().c_str();
            }
        }

        filter[filter_count] = NULL;
        return error.code;
    }

    /*
     **********************************************************************************************
     * Helper function to get filtered/select bins from server.
     *
     * @param php_filter_bins   PHP Array reference to the php_filter_bins to be
     *                          selected by the get/getMany operation.
     * @param as_p              aerospike pointer to be used by this select
     *                          operation.
     * @param read_policy_p     The as_policy_read to be used by this select
     *                          operation.
     * @param key               The as_key reference for the record to select.
     * @param record_pp         The return as_record double pointer to be
     *                          populated by this function with the record
     *                          fetched using the select operation.
     * @param error             The as_error reference to be populated by this
     *                          function in case of error.
     *
     * @return AEROSPIKE_OK if SUCCESS. Otherwise AEROSPIKE_ERR_*.
     **********************************************************************************************
     */
    as_status aerospike_get_filtered_bins(const Array& php_filter_bins,
            aerospike *as_p, as_policy_read *read_policy_p,
            as_key& key, as_record **record_pp, as_error& error)
    {
        uint16_t            total_filter_count = php_filter_bins.size();
        const char          *filter[total_filter_count];

        as_error_reset(&error);

        if (AEROSPIKE_OK == process_filter_bins(php_filter_bins, filter, error)) {
            aerospike_key_select(as_p, &error, read_policy_p, &key, filter, record_pp);
        }
        return error.code;
    }

} // namespace HPHP
