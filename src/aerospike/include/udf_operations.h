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
    extern as_status register_udf_module(aerospike *as_p, const Variant&lua_path, const Variant& lua_module, const Variant& language, as_policy_info *info_policy_p, as_error& error);
    extern as_status remove_udf_module(aerospike *as_p, const Variant& lua_module, as_policy_info *info_policy_p, as_error& error);
    extern as_status aerospike_udf_apply(aerospike *as_p, as_key key, const Variant& lua_module, const Variant& lua_function, const Variant& lua_args, as_policy_apply *apply_policy_p, StaticPoolManager &static_pool, int16_t serializer_type, VRefParam returned_type, as_error& error);
} // namespace HPHP
#endif /* end of __UDF_OPERATIONS_H__ */
