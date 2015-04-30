#ifndef __CONVERSIONS_H__
#define __CONVERSIONS_H__

#include "hphp/runtime/ext/extension.h"
#include "hphp/runtime/base/execution-context.h"
#include "hphp/runtime/vm/native-data.h"

extern "C" {
#include "aerospike/aerospike_key.h"
#include "aerospike/as_status.h"
#include "aerospike/as_record.h"
#include "aerospike/as_list.h"
#include "aerospike/as_arraylist.h"
#include "aerospike/as_map.h"
#include "aerospike/as_hashmap.h"
}

namespace HPHP {
    class StaticPoolManager;
    /*
     *************************************************************************************************
     * Declaration of functions in conversions.cpp
     *************************************************************************************************
     */
    extern as_status php_config_to_as_config(const Array& php_config, as_config& config, as_error& error);
    extern as_status php_key_to_as_key(const Array& php_key, as_key& key, as_error& error);
    extern as_status php_record_to_as_record(const Array& php_record, as_record& record, int64_t ttl, StaticPoolManager& static_pool, as_error& error);
    extern as_status php_variant_to_as_val(const Variant& php_variant, as_val **val_pp, StaticPoolManager& static_pool, as_error& error);
    extern as_status php_list_to_as_list(const Array& php_list, as_list **list_pp, StaticPoolManager& static_pool, as_error& error);
    extern as_status php_map_to_as_map(const Array& php_map, as_map **map_pp, StaticPoolManager& static_pool, as_error& error);
    extern as_status php_operations_to_as_operations(const Array& php_operations, as_operations& operations, StaticPoolManager& static_pool, as_error& error);
    extern as_status as_record_to_php_record(const as_record *record_p, const as_key *key_p, VRefParam php_rec, as_policy_read *policy_p, as_error& error);
    extern as_status as_val_to_php_variant(const as_val *value_p, Variant& php_value, as_error& error);
    extern as_status as_list_to_php_list(const as_list *list_p, Variant& php_list, as_error& error);
    extern as_status as_map_to_php_map(const as_map *map_p, Variant& php_map, as_error& error);
    extern as_status bins_to_php_bins(const as_record *record_p, Array& php_bins, as_error& error);

    static const int PHP_KEY_SIZE = 3;

    /*
     **************************************************************************************************
     * Class to manage a static pool of as_string, as_integer, as_arraylist, as_hashmap for use in
     * the flow of conversions.
     * Instantiate this class and invoke methods to make use of stack allocated
     * as_* datatypes in order to avoid mallocs via use of as_*_new() APIs.
     **************************************************************************************************
     */

    class StaticPoolManager {
        private:
            static const uint32_t       STATIC_POOL_MAX_SIZE = 1024;
            as_string                   string_pool[STATIC_POOL_MAX_SIZE];
            as_integer                  integer_pool[STATIC_POOL_MAX_SIZE];
            as_arraylist                list_pool[STATIC_POOL_MAX_SIZE];
            as_hashmap                  map_pool[STATIC_POOL_MAX_SIZE];
            uint32_t                    string_pool_index = 0;
            uint32_t                    integer_pool_index = 0;
            uint32_t                    list_pool_index = 0;
            uint32_t                    map_pool_index = 0;

        public:
            StaticPoolManager();
            as_string* get_as_string();
            as_integer* get_as_integer();
            as_arraylist* get_as_arraylist();
            as_hashmap* get_as_hashmap();
            ~StaticPoolManager();
    };

    /*
     ************************************************************************************
     * Structure declaration for foreach_callback_udata.
     * Holds the 'data' to be populated by the callback, and 'error' to be
     * populated in case of errors.
     ************************************************************************************
     */
    typedef struct __foreach_callback_udata {
        Array& data;
        as_error& error;
        __foreach_callback_udata(Array &init_data, as_error& init_error) : data(init_data), error(init_error) {}
    } foreach_callback_udata;
} // namespace HPHP
#endif /* end of __CONVERSIONS_H__ */
