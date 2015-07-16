#ifndef __UDF_OPERATIONS_H__
#define __UDF_OPERATIONS_H__

#include "hphp/runtime/ext/extension.h"
#include "hphp/runtime/base/execution-context.h"
#include "hphp/runtime/vm/native-data.h"
#include "hphp/runtime/base/builtin-functions.h"

extern "C" {
#include "aerospike/aerospike_key.h"
#include "aerospike/as_status.h"
#include "aerospike/as_record.h"
#include "aerospike/as_list.h"
#include "aerospike/as_arraylist.h"
#include "aerospike/aerospike_udf.h"
}


namespace HPHP {
#define LUA_FILE_BUFFER_FRAME 512
    /*
     *************************************************************************************************
     * Declaration of functions in udf_operations.cpp
     *************************************************************************************************
     */
    class StaticPoolManager;
    extern as_status register_udf_module(aerospike *as_p, const Variant& path, const Variant& module, const Variant& language, as_policy_info *info_policy_p, as_error& error);
    extern as_status remove_udf_module(aerospike *as_p, const Variant& module, as_policy_info *info_policy_p, as_error& error);
    extern as_status get_registered_udf_module_code(aerospike *as_p, const Variant& module, String &module_code, const Variant& language, as_policy_info *info_policy_p, as_error& error);
    extern as_status list_registered_udf_modules(aerospike *as_p, Array& modules, const Variant& language, as_policy_info *info_policy_p, as_error& error);
    extern as_status aerospike_udf_apply(aerospike *as_p, as_key key, const Variant& module, const Variant& function, const Variant& args, as_policy_apply *apply_policy_p, StaticPoolManager &static_pool, int16_t serializer_type, VRefParam php_returned_value, as_error& error);
} // namespace HPHP
#endif /* end of __UDF_OPERATIONS_H__ */
