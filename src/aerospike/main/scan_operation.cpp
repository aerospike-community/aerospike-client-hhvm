#include "scan_operation.h"
#include "conversions.h"
#include "hphp/runtime/base/builtin-functions.h"

namespace HPHP {

    /*
     *******************************************************************************************
     * Callback function for each record scanned by aerospike_scan_foreach() API
     *
     * @param val_p         An as_val of record type
     * @param udata_p       foreach_callback_user_udata pointer which contains
     *                      PHP user callback function and as_error reference
     *                      to be populated in case of error.
     * @return AEROSPIKE_OK if success. Otherwise AEROSPIKE_ERR_*.
     *******************************************************************************************
     */
    bool scan_callback(const as_val * val_p, void * udata)
    {
        if (!val_p) {
            return false;
        }

        as_record       *record_p = as_record_fromval(val_p);

        if (!record_p) {
            return false;
        }

        pthread_rwlock_wrlock(&scan_callback_mutex);
        Array           temp_php_record = Array::Create();
        foreach_callback_user_udata      *conversion_data_p = (foreach_callback_user_udata *)udata;

        as_record_to_php_record(record_p, &record_p->key, temp_php_record, NULL, conversion_data_p->error);

        Array php_record = Array::Create();
        php_record.append(temp_php_record);
        Variant ret = vm_call_user_func(conversion_data_p->function, php_record);

        bool do_continue = true;
        if (ret.isBoolean() && ret.toBoolean() == false) {
            do_continue = false;
        }

        as_record_destroy(record_p);
        pthread_rwlock_unlock(&scan_callback_mutex);

        return do_continue;
    }

    /*
     *******************************************************************************************
     * Function to initialize as_scan structure for calling aerospike_scan_foreach() API
     *
     * @param scan          An as_scan pointer
     * @param ns            Namespace to be scanned
     * @param set           Set to be scanned
     * @param bins          Array of Bins to be scanned
     * @param error         as_error reference to be populated by this function
     *                      in case of error
     * @return AEROSPIKE_OK if success. Otherwise AEROSPIKE_ERR_*.
     *******************************************************************************************
     */
    as_status initialize_scan(as_scan *scan, const Variant &ns, const Variant &set, const Variant &bins, as_error &error)
    {
        as_error_reset(&error);

        if (!ns.isString() || ns.toString().empty() || !set.isString() || set.toString().empty()) {
            as_error_update(&error, AEROSPIKE_ERR_PARAM,
                    "Namespace/Set must be non empty string");
        } else if (!bins.isNull() && !bins.isArray()) {
            as_error_update(&error, AEROSPIKE_ERR_PARAM,
                    "Bin names must be an Array");
        } else {
            //Initialize scan structure
            as_scan_init(scan, ns.toString().c_str(), set.toString().c_str());
            //Add bins to structure if presents
            if (!bins.isNull()) {
                Array bin_array = bins.toArray();
                //Initialize select bins only when argument contails bins
                if (bin_array.length() > 0) {
                    as_scan_select_init(scan, bin_array.length());
                    for (ArrayIter iter(bin_array); iter; ++iter) {
                        if (!iter.second().isArray()) {
                            as_scan_select(scan, iter.second().toString().c_str());
                        } else {
                            //In case bin names are arrays currently we are ignoring
                            //those bins
                            continue;
                            //For generating error for above case, enable below
                            //error code
                            //as_error_update(&error, AEROSPIKE_ERR_PARAM,
                            //        "Bin names must be an Array of Strings");
                            //break;
                        }
                    }
                }
            }
        }

        return error.code;
    }

    /*
     *******************************************************************************************
     * Function to initialize as_scan structure for calling
     * aerospike_scan_background() API
     *
     * @param scan              An as_scan pointer
     * @param ns                Namespace to be scanned
     * @param set               Set to be scanned
     * @param module            Module registered in DB
     * @param function          Function defined in above registered module
     * @param args              List of arguments to the above defined function
     * @param static_pool       StaticPoolManager instance reference, to be used for
     *                          the conversion lifecycle
     * @param error             as_error reference to be populated by this function
     *                          in case of error
     * @return AEROSPIKE_OK if success. Otherwise AEROSPIKE_ERR_*.
     *******************************************************************************************
     */
    as_status initialize_scanApply(as_scan *scan, const Variant &ns, const Variant &set, const Variant &module, const Variant &function, const Variant &args, StaticPoolManager &static_pool, as_error &error)
    {
        as_list     *args_list = NULL;
        Array       php_args;

        as_error_reset(&error);

        if (!ns.isString() || ns.toString().empty() || !set.isString() || set.toString().empty()) {
            as_error_update(&error, AEROSPIKE_ERR_PARAM,
                    "Namespace/Set must not be empty");
        } else if (!module.isString() || module.toString().empty() || !function.isString() || function.toString().empty()) {
            as_error_update(&error, AEROSPIKE_ERR_PARAM,
                    "Module/Function must not be empty");
        } else if (!args.isNull() && !args.isArray()) {
            as_error_update(&error, AEROSPIKE_ERR_PARAM,
                    "Function arguments must be of type array");
        } else {
            //Initialize scan structure
            as_scan_init(scan, ns.toString().c_str(), set.toString().c_str());

            if (!args.isNull()) {
                php_args = args.toArray();
            }
            if (!args.isNull() && php_list_to_as_list(php_args, &args_list, static_pool, error)) {
                //Argument list creation for UDF failed
            } else if (!as_scan_apply_each(scan, module.toString().c_str(), function.toString().c_str(), args_list)) {
                as_error_update(&error, AEROSPIKE_ERR_PARAM,
                        "Unable to initiate background scan : UDF apply failed");
            }
        }

        return error.code;
    }

}
