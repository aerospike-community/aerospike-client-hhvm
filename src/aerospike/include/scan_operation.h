#ifndef __SCAN_OPERATION_H__
#define __SCAN_OPERATION_H__

#include "hphp/runtime/ext/extension.h"
#include "hphp/runtime/base/execution-context.h"
#include "hphp/runtime/vm/native-data.h"

extern "C" {
#include "aerospike/as_status.h"
#include "aerospike/as_record.h"
#include "aerospike/as_list.h"
#include "aerospike/as_arraylist.h"
#include "aerospike/as_scan.h"
}

namespace HPHP {

    /*
     *******************************************************************************************
     * Declaration of mutex used in case of CONCURRENT policy for
     * aerospike_scan_foreach()
     *******************************************************************************************
     */
    extern pthread_rwlock_t scan_callback_mutex;

    /*
     *******************************************************************************************
     * Declaration of function to initialize scan API required CSDK structures
     *******************************************************************************************
     */
    class StaticPoolManager;
    extern bool scan_callback(const as_val * val_p, void * udata);
    extern as_status initialize_scan(as_scan *scan, const Variant &ns, const Variant &set, const Variant &bins, as_error &error);
    extern as_status initialize_scanApply(as_scan *scan, const Variant &ns, const Variant &set, const Variant &module, const Variant &function, const Variant &args, StaticPoolManager &static_pool, int16_t serializer_type, as_error &error);

    /*
     ************************************************************************************
     * Structure declaration for foreach_callback_user_udata.
     * Holds the 'function' to be populated by the callback, and 'error' to be
     * populated in case of errors.
     ************************************************************************************
     */
    typedef struct __foreach_callback_user_udata {
        const Variant& function;
        as_error& error;
        __foreach_callback_user_udata(const Variant &init_data, as_error& init_error) : function(init_data), error(init_error) {}
    } foreach_callback_user_udata;
} //namespace HPHP
#endif /* end of __SCAN_OPERATION_H__ */
