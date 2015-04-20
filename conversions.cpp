#include "conversions.h"
#include "ext_aerospike.h"

namespace HPHP {

    /*
     *******************************************************************************************
     * Definitions of Member functions of Class StaticPoolManager declared in
     * conversions.h
     *******************************************************************************************
     */

    /*
     *******************************************************************************************
     * Constructor for static pool
     *******************************************************************************************
     */
    StaticPoolManager::StaticPoolManager() { }

    /*
     *******************************************************************************************
     * Method to get an as_string from current static pool
     *
     * @return a pointer to as_string if success. Otherwise NULL.
     *******************************************************************************************
     */
    as_string* StaticPoolManager::get_as_string()
    {
        if (string_pool_index >= STATIC_POOL_MAX_SIZE) {
            return NULL;
        }
        return &string_pool[string_pool_index++];
    }

    /*
     *******************************************************************************************
     * Method to get an as_integer from current static pool
     *
     * @return a pointer to as_integer if success. Otherwise NULL.
     *******************************************************************************************
     */
    as_integer* StaticPoolManager::get_as_integer()
    {
        if (integer_pool_index >= STATIC_POOL_MAX_SIZE) {
            return NULL;
        }
        return &integer_pool[integer_pool_index++];
    }

    /*
     *******************************************************************************************
     * Method to get an as_arraylist from current static pool
     *
     * @return a pointer to as_arraylist if success. Otherwise NULL.
     *******************************************************************************************
     */
    as_arraylist* StaticPoolManager::get_as_arraylist()
    {
        if (list_pool_index >= STATIC_POOL_MAX_SIZE) {
            return NULL;
        }
        return &list_pool[list_pool_index++];
    }

    /*
     *******************************************************************************************
     * Method to get an as_hashmap from current static pool
     *
     * @return a pointer to as_hashmap if success. Otherwise NULL.
     *******************************************************************************************
     */
    as_hashmap* StaticPoolManager::get_as_hashmap()
    {
        if (map_pool_index >= STATIC_POOL_MAX_SIZE) {
            return NULL;
        }
        return &map_pool[map_pool_index++];
    }

    /*
     *******************************************************************************************
     * Destructor for Static Pool
     * Frees up all the used as_* from the static pool
     *******************************************************************************************
     */
    StaticPoolManager::~StaticPoolManager()
    {
        uint16_t iter = 0;
        for (iter = 0; iter < string_pool_index; iter++) {
            as_string_destroy(&string_pool[iter]);
        }
        for (iter = 0; iter < integer_pool_index; iter++) {
            as_integer_destroy(&integer_pool[iter]);
        }
        for (iter = 0; iter < list_pool_index; iter++) {
            as_arraylist_destroy(&list_pool[iter]);
        }
        for (iter = 0; iter < map_pool_index; iter++) {
            as_hashmap_destroy(&map_pool[iter]);
        }
    }

    /*
     *******************************************************************************************
     * Conversion APIs
     *******************************************************************************************
     */

    /*
     *******************************************************************************************
     * Function to convert PHP config array into as_config
     *
     * @param php_config    PHP Array reference that holds client configuration
     * @param config        as_config reference to be populated by this function
     * @param error         as_error reference to be populated by this function
     *                      in case of error
     * @return AEROSPIKE_OK if success. Otherwise AEROSPIKE_ERR_*.
     *******************************************************************************************
     */
    as_status php_config_to_as_config(const Array& php_config, as_config& config, as_error& error)
    {
        as_error_reset(&error);

        if (!php_config.exists(s_hosts)) {
            return as_error_update(&error, AEROSPIKE_ERR_PARAM,
                    "Invalid config array: hosts key not found");
        }

        if (!(php_config[s_hosts].isArray())) {
            return as_error_update(&error, AEROSPIKE_ERR_PARAM,
                    "Invalid config array: Value of hosts is expected to be an array");
        }

        Array hosts_array = php_config[s_hosts].toArray();
        as_config_init(&config);

        for (uint16_t i = 0; i < hosts_array.length(); i++) {
            if (!hosts_array[i].isArray()) {
                return as_error_update(&error, AEROSPIKE_ERR_PARAM,
                        "Invalid hosts array: Value of each host is expected to be an array of addr, port");
            }

            Array host = hosts_array[i].toArray();
            if (!host.exists(s_addr) ||
                    !host[s_addr].isString() ||
                    !host.exists(s_port) ||
                    !host[s_port].isInteger()) {
                return as_error_update(&error, AEROSPIKE_ERR_PARAM,
                        "Invalid hosts array: Value of each host is expected to be an array of addr, port");
            }

            config.hosts[i].addr = host[s_addr].toString().c_str();
            config.hosts[i].port = host[s_port].toInt64();
        }
        
        return error.code;
    }

    /*
     *******************************************************************************************
     * Function to convert PHP key array into as_key
     *
     * @param php_key       PHP Array reference that holds record's key
     * @param key           as_key reference to be populated by this function
     * @param error         as_error reference to be populated by this function
     *                      in case of error
     * @return AEROSPIKE_OK if success. Otherwise AEROSPIKE_ERR_*.
     *******************************************************************************************
     */
    as_status php_key_to_as_key(const Array& php_key, as_key& key, as_error& error)
    {
        const char      *ns_p = NULL;
        const char      *set_p = NULL;

        as_error_reset(&error);

        if (PHP_KEY_SIZE != php_key.size() ||
                !php_key.exists(s_ns) ||
                !php_key.exists(s_set) ||
                !php_key.exists(s_key)) {
            return as_error_update(&error, AEROSPIKE_ERR_PARAM,
                    "PHP Key must be an assoc array(ns, set, key)");
        }

        if (!(php_key[s_ns].isString())) {
            return as_error_update(&error, AEROSPIKE_ERR_PARAM,
                    "Invalid namespace: Expecting a string");
        }

        ns_p = php_key[s_ns].toString().c_str();

        if (!(php_key[s_set].isString())) {
            return as_error_update(&error, AEROSPIKE_ERR_PARAM,
                    "Invalid set: Expecting a string");
        }
        
        set_p = php_key[s_set].toString().c_str();

        if (php_key[s_key].isInteger()) {
            if (!as_key_init_int64(&key, ns_p, set_p, php_key[s_key].toInt64())) {
                return as_error_update(&error, AEROSPIKE_ERR_CLIENT,                   
                        "Unable to initialize integer as_key")
            }
        } else if (php_key[s_key].isString()) {
            if (!as_key_init_str(&key, ns_p, set_p, php_key[s_key].toString().c_str())) {
                return as_error_update(&error, AEROSPIKE_ERR_CLIENT,                   
                        "Unable to initialize string as_key")
            }
        } else {
            return as_error_update(&error, AEROSPIKE_ERR_PARAM,
                    "Invalid key: Expecting a string/integer");
        }

        return error.code;
    }

    /*
     *******************************************************************************************
     * Function to determine whether a given PHP Array reference holds reference of an
     * associative array or indexed array.
     *
     * @param php_array     PHP Array reference, the type of which is to be determined
     * @return true if associative. Otherwise false if indexed.
     *******************************************************************************************
     */
    static bool is_assoc(Array& php_array)
    {
        uint16_t iter_count = 0;
        for (ArrayIter iter(php_array); iter; ++iter) {
            if (iter.first().isInteger() && iter.first().toInt64() == iter_count) {
                iter_count++;
            } else {
                return true;
            }
        }
        return false;
    }

    /*
     *******************************************************************************************
     * Function to convert PHP variant into as_val
     *
     * @param php_variant   PHP Variant reference that is to be converted
     * @param val_pp        as_val pointer to be allocated and populated by this function
     * @param static_pool   StaticPoolManager instance reference, to be used for
     *                      the conversion lifecycle.
     * @param error         as_error reference to be populated by this function
     *                      in case of error
     * @return AEROSPIKE_OK if success. Otherwise AEROSPIKE_ERR_*.
     *******************************************************************************************
     */
    as_status php_variant_to_as_val(const Variant& php_variant, as_val **val_pp, StaticPoolManager& static_pool, as_error& error)
    {
        as_error_reset(&error);

        if (!val_pp) {
            return as_error_update(&error, AEROSPIKE_ERR_CLIENT,
                    "Variant value is null");
        }

        if (php_variant.isInteger()) {
            if (NULL == (*val_pp = (as_val *) static_pool.get_as_integer())) {
                return as_error_update(&error, AEROSPIKE_ERR_CLIENT,
                        "StaticPoolManager failed to allocate as_integer: Upper limit of size exceeded!")
            }
            *val_pp = (as_val *) as_integer_init((as_integer *) *val_pp, (int64_t) php_variant.toInt64());
        } else if (php_variant.isString()) {
            if (NULL == (*val_pp = (as_val *) static_pool.get_as_string())) {
                return as_error_update(&error, AEROSPIKE_ERR_CLIENT,
                        "StaticPoolManager failed to allocate as_string: Upper limit of size exceeded!")
            }
            *val_pp = (as_val *) as_string_init((as_string *) *val_pp, (char *) php_variant.toString().c_str(), false);
        } else if (php_variant.isArray()) {
            Array php_array = php_variant.toArray();

            if (is_assoc(php_array)) {
                /* Handle map */
                as_map *map_p = NULL;
                if (AEROSPIKE_OK != php_map_to_as_map(php_array, &map_p, static_pool, error)) {
                    return error.code;
                }
                *val_pp = (as_val *) map_p;
            } else {
                /* Handle list */
                as_list *list_p = NULL;
                if (AEROSPIKE_OK != php_list_to_as_list(php_array, &list_p, static_pool, error)) {
                    return error.code;
                }
                *val_pp = (as_val *) list_p;
            }
        } else {
            return as_error_update(&error, AEROSPIKE_ERR_PARAM,
                    "Unsupported type of value");
        }
        return error.code;
    }

    /*
     *******************************************************************************************
     * Function to convert PHP list into as_list
     *
     * @param php_list      PHP Array reference that is to be converted
     * @param list_pp       as_list pointer to be allocated and populated by this function
     * @param static_pool   StaticPoolManager instance reference, to be used for
     *                      the conversion lifecycle.
     * @param error         as_error reference to be populated by this function
     *                      in case of error
     * @return AEROSPIKE_OK if success. Otherwise AEROSPIKE_ERR_*.
     *******************************************************************************************
     */
    as_status php_list_to_as_list(const Array& php_list, as_list **list_pp, StaticPoolManager& static_pool, as_error& error)
    {
        as_error_reset(&error);

        if (!list_pp) {
            return as_error_update(&error, AEROSPIKE_ERR_CLIENT,
                    "List is null");
        }

        if (NULL == *list_pp) {
            if (NULL == (*list_pp = (as_list *) static_pool.get_as_arraylist())) {
                return as_error_update(&error, AEROSPIKE_ERR_CLIENT,
                        "StaticPoolManager failed to allocate as_arraylist: Upper limit of size exceeded!")
            }
            *list_pp = (as_list *) as_arraylist_init((as_arraylist *) *list_pp, php_list.length(), 0);
        }

        for (ArrayIter iter(php_list); iter; ++iter) {
            Variant php_value = iter.second();
            as_val *val_p = NULL;
            if (AEROSPIKE_OK != php_variant_to_as_val(php_value, &val_p, static_pool, error)) {
                break;
            }
            as_list_append(*list_pp, val_p);
        }

        if (AEROSPIKE_OK != error.code) {
            as_list_destroy(*list_pp);
        }
        return error.code;
    }

    /*
     *******************************************************************************************
     * Function to convert PHP Map into as_map
     *
     * @param php_map       PHP Array reference that is to be converted
     * @param map_pp        as_map pointer to be allocated and populated by this function
     * @param static_pool   StaticPoolManager instance reference, to be used for
     *                      the conversion lifecycle.
     * @param error         as_error reference to be populated by this function
     *                      in case of error
     * @return AEROSPIKE_OK if success. Otherwise AEROSPIKE_ERR_*.
     *******************************************************************************************
     */
    as_status php_map_to_as_map(const Array& php_map, as_map **map_pp, StaticPoolManager& static_pool, as_error& error)
    {
        as_error_reset(&error);

        if (!map_pp) {
            return as_error_update(&error, AEROSPIKE_ERR_CLIENT,
                    "Map is null");
        }

        if (NULL == *map_pp) {
            if (NULL == (*map_pp = (as_map *) static_pool.get_as_hashmap())) {
                return as_error_update(&error, AEROSPIKE_ERR_CLIENT,
                        "StaticPoolManager failed to allocate as_hashmap: Upper limit of size exceeded!");
            }
            *map_pp = (as_map *) as_hashmap_init((as_hashmap *) *map_pp, php_map.length());
        }

        for (ArrayIter iter(php_map); iter; ++iter) {
            Variant php_key = iter.first();
            as_val *key_p = NULL;
            if (AEROSPIKE_OK != php_variant_to_as_val(php_key, &key_p, static_pool, error)) {
                break;
            }
            Variant php_value = iter.second();
            as_val *val_p = NULL;
            if (AEROSPIKE_OK != php_variant_to_as_val(php_value, &val_p, static_pool, error)) {
                if (key_p) {
                    as_val_destroy(key_p);
                }
                break;
            }
            as_map_set(*map_pp, key_p, val_p);
        }

        if (AEROSPIKE_OK != error.code) {
            as_map_destroy(*map_pp);
        }
        return error.code;
    }

    /*
     *******************************************************************************************
     * Function to convert PHP record into as_record
     *
     * @param php_record    PHP Array reference that is to be converted
     * @param record        as_record reference to be populated by this function
     * @param static_pool   StaticPoolManager instance reference, to be used for
     *                      the conversion lifecycle.
     * @param error         as_error reference to be populated by this function
     *                      in case of error
     * @return AEROSPIKE_OK if success. Otherwise AEROSPIKE_ERR_*.
     *******************************************************************************************
     */
    as_status php_record_to_as_record(const Array& php_record, as_record& record, StaticPoolManager& static_pool, as_error& error)
    {
        const char      *bin_name_p = NULL;

        as_error_reset(&error);

        as_record_init(&record, php_record.size());

        for (ArrayIter iter(php_record); iter; ++iter) {
            Variant key = iter.first();
            if (key.isString()) {
                bin_name_p = key.toString().c_str();
            } else {
                return as_error_update(&error, AEROSPIKE_ERR_PARAM,
                        "Invalid Bin Name type, expecting a string");
            }
            Variant value = iter.second();
            if (value.isString()) {
                if (!as_record_set_str(&record, bin_name_p, value.toString().c_str())) {
                    return as_error_update(&error, AEROSPIKE_ERR_PARAM,
                        "Unable to set string value within as_record");
                }
            } else if (value.isInteger()) {
                if (!as_record_set_int64(&record, bin_name_p, value.toInt64())) {
                    return as_error_update(&error, AEROSPIKE_ERR_PARAM,
                        "Unable to set integer value within as_record");
                }
            } else if (value.isArray()) {
                Array php_array = value.toArray();
                if (is_assoc(php_array)) {
                    /* Handle map */
                    as_map *map_p = NULL;
                    if (AEROSPIKE_OK != php_map_to_as_map(php_array, &map_p, static_pool, error)) {
                        break;
                    }
                    if (!as_record_set_map(&record, bin_name_p, map_p)) {
                        return as_error_update(&error, AEROSPIKE_ERR_PARAM,
                                "Unable to set map value within as_record");
                    }
                } else {
                    /* Handle list */
                    as_list *list_p = NULL;
                    if (AEROSPIKE_OK != php_list_to_as_list(php_array, &list_p, static_pool, error)) {
                        break;
                    }
                    if (!as_record_set_list(&record, bin_name_p, list_p)) {
                        return as_error_update(&error, AEROSPIKE_ERR_PARAM,
                                "Unable to set list value within as_record");
                    }
                }
            } else {
                return as_error_update(&error, AEROSPIKE_ERR_PARAM,
                    "Unsupported type of value for the record");
            }
        }
        return error.code;
    }

    /*
     *******************************************************************************************
     * Callback function for each as_list element
     * Appends corresponding PHP value for current as_value (element of the
     * as_list) to the PHP list passed within udata_p
     *
     * @param value_p       An as_val list element
     * @param udata_p       foreach_callback_udata pointer which wraps us the PHP list
     *                      to be appended to by this function and as_error reference
     *                      to be populated in case of error.
     * @return AEROSPIKE_OK if success. Otherwise AEROSPIKE_ERR_*.
     *******************************************************************************************
     */
    static bool list_to_php_list_foreach_callback(as_val *value_p, void *udata_p)
    {
        if (!value_p) {
            return false;
        }

        foreach_callback_udata *conversion_data_p = (foreach_callback_udata *) udata_p;
        Variant php_value;
        if (AEROSPIKE_OK != as_val_to_php_variant(value_p, php_value, conversion_data_p->error)) {
            return false;
        }

        conversion_data_p->data.append(php_value);
        return true;
    }

    /*
     *******************************************************************************************
     * Function to convert as_list into PHP list
     *
     * @param list_p        as_list to be converted by this function
     * @param php_list      PHP Variant reference to be populated by this
     *                      function
     * @param error         as_error reference to be populated by this function
     *                      in case of error
     * @return AEROSPIKE_OK if success. Otherwise AEROSPIKE_ERR_*.
     *******************************************************************************************
     */
    as_status as_list_to_php_list(const as_list *list_p, Variant& php_list, as_error& error)
    {
        as_error_reset(&error);
        if (!list_p) {
            return as_error_update(&error, AEROSPIKE_ERR_CLIENT,
                    "List is null");
        }
        Array                       temp_php_list;
        foreach_callback_udata      udata(temp_php_list, error);
        as_list_foreach(list_p, (as_list_foreach_callback) list_to_php_list_foreach_callback, &udata);
        php_list = temp_php_list;
        return error.code;
    }

    /*
     *******************************************************************************************
     * Callback function for each as_map element
     * Sets corresponding PHP values for current as_values (key and value of the
     * as_map) within the PHP map passed within udata_p
     *
     * @param key_p         An as_val map key
     * @param value_p       An as_val map value
     * @param udata_p       foreach_callback_udata pointer which wraps us the PHP map
     *                      to be set by this function and as_error reference
     *                      to be populated in case of error.
     * @return AEROSPIKE_OK if success. Otherwise AEROSPIKE_ERR_*.
     *******************************************************************************************
     */
    static bool map_to_php_map_foreach_callback(as_val *key_p, as_val *value_p, void *udata_p)
    {
        if (!key_p || !value_p) {
            return false;
        }

        foreach_callback_udata *conversion_data_p = (foreach_callback_udata *) udata_p;
        Variant php_key;
        if (AEROSPIKE_OK != as_val_to_php_variant(key_p, php_key, conversion_data_p->error)) {
            return false;
        }

        Variant php_value;
        if (AEROSPIKE_OK != as_val_to_php_variant(value_p, php_value, conversion_data_p->error)) {
            return false;
        }

        conversion_data_p->data.set(php_key, php_value);
        return true;
    }

    /*
     *******************************************************************************************
     * Function to convert as_map into PHP map
     *
     * @param map_p         as_map to be converted by this function
     * @param php_map       PHP Variant reference to be populated by this
     *                      function
     * @param error         as_error reference to be populated by this function
     *                      in case of error
     * @return AEROSPIKE_OK if success. Otherwise AEROSPIKE_ERR_*.
     *******************************************************************************************
     */
    as_status as_map_to_php_map(const as_map *map_p, Variant& php_map, as_error& error)
    {
        as_error_reset(&error);
        if (!map_p) {
            return as_error_update(&error, AEROSPIKE_ERR_CLIENT,
                    "Map is null");
        }
        Array                       temp_php_map;
        foreach_callback_udata      udata(temp_php_map, error);
        as_map_foreach(map_p, (as_map_foreach_callback) map_to_php_map_foreach_callback, &udata);
        php_map = temp_php_map;
        return error.code;
    }

    /*
     *******************************************************************************************
     * Function to convert as_val into PHP Variant
     *
     * @param value_p       as_val to be converted by this function
     * @param php_value     PHP Variant reference to be populated by this
     *                      function
     * @param error         as_error reference to be populated by this function
     *                      in case of error
     * @return AEROSPIKE_OK if success. Otherwise AEROSPIKE_ERR_*.
     *******************************************************************************************
     */
    as_status as_val_to_php_variant(const as_val *value_p, Variant& php_value, as_error& error)
    {
        as_error_reset(&error);
        switch(as_val_type(value_p)) {
            case AS_STRING:
                {
                    as_string *string_p = as_string_fromval(value_p);
                    if (!string_p) {
                        return as_error_update(&error, AEROSPIKE_ERR_CLIENT,
                                "String is null");
                    }
                    php_value = String(as_string_get(string_p));
                    break;
                }
            case AS_INTEGER:
                {
                    as_integer *integer_p = as_integer_fromval(value_p);
                    if (!integer_p) {
                        return as_error_update(&error, AEROSPIKE_ERR_CLIENT,
                                "Integer is null");
                    }
                    php_value = as_integer_get(integer_p);
                    break;
                }
            case AS_BYTES:
                {
                    /* TBD */
                    break;
                }
            case AS_LIST:
                {
                    as_list *list_p = as_list_fromval((as_val *) value_p);
                    as_list_to_php_list(list_p, php_value, error);
                    break;
                }
            case AS_MAP:
                {
                    as_map *map_p = as_map_fromval((as_val *) value_p);
                    as_map_to_php_map(map_p, php_value, error);
                    break;
                }
            case AS_REC:
                {
                    as_record *record_p = as_record_fromval(value_p);
                    as_record_to_php_record(record_p, NULL, php_value, error);
                    break;
                }
            case AS_NIL:
                {
                    break;
                }
            default:
                {
                    as_error_update(&error, AEROSPIKE_ERR_CLIENT, "Unsupported Datatype for Bin");
                }
        }
        return error.code;
    }

    /*
     *******************************************************************************************
     * Callback function for each bin-value within as_record
     * Sets corresponding PHP values for current bin name and as_value within the 
     * PHP bin array passed within udata_p
     *
     * @param name_p        Current bin name
     * @param value_p       Current as_val bin value
     * @param udata_p       foreach_callback_udata pointer which wraps us the PHP bins
     *                      to be set by this function and as_error reference
     *                      to be populated in case of error.
     * @return AEROSPIKE_OK if success. Otherwise AEROSPIKE_ERR_*.
     *******************************************************************************************
     */
    static bool bins_to_php_bins_foreach_callback(const char *name_p, const as_val *value_p, void *udata_p)
    {
        if (!name_p || !value_p) {
            return false;
        }

        foreach_callback_udata *conversion_data_p = (foreach_callback_udata *) udata_p;
        Variant php_value;
        if (AEROSPIKE_OK != as_val_to_php_variant(value_p, php_value, conversion_data_p->error)) {
            return false;
        }

        conversion_data_p->data.set(String(name_p), php_value);
        return true;
    }

    /*
     *******************************************************************************************
     * Function to convert bins of as_record into PHP bins Array
     *
     * @param record_p      as_record, the bins of which are to be converted by this function
     * @param php_bins      PHP Array reference to be populated by this
     *                      function
     * @param error         as_error reference to be populated by this function
     *                      in case of error
     * @return AEROSPIKE_OK if success. Otherwise AEROSPIKE_ERR_*.
     *******************************************************************************************
     */
    static as_status bins_to_php_bins(const as_record *record_p, Array& php_bins, as_error& error)
    {
        as_error_reset(&error);
        if (!record_p) {
            return as_error_update(&error, AEROSPIKE_ERR_CLIENT,
                    "Record is null");
        }

        foreach_callback_udata      udata(php_bins, error);
        as_record_foreach(record_p, (as_rec_foreach_callback) bins_to_php_bins_foreach_callback, &udata);
        return error.code;
    }

    /*
     *******************************************************************************************
     * Function to convert metadata of as_record into PHP metadata Array
     *
     * @param record_p      as_record, the metadata of which is to be converted by this function
     * @param php_metadata  PHP Array reference to be populated by this
     *                      function
     * @param error         as_error reference to be populated by this function
     *                      in case of error
     * @return AEROSPIKE_OK if success. Otherwise AEROSPIKE_ERR_*.
     *******************************************************************************************
     */
    static as_status metadata_to_php_metadata(const as_record *record_p, Array& php_metadata, as_error& error)
    {
        as_error_reset(&error);
        if (!record_p) {
            return as_error_update(&error, AEROSPIKE_ERR_CLIENT,
                    "Record is null");
        }
        php_metadata.set(s_ttl, uint64_t(record_p->ttl));
        php_metadata.set(s_generation, uint64_t(record_p->gen));
        return error.code;
    }

    /*
     *******************************************************************************************
     * Function to convert as_key into PHP key Array
     *
     * @param key_p         as_key to be converted by this function
     * @param php_key       PHP Array reference to be populated by this
     *                      function
     * @param error         as_error reference to be populated by this function
     *                      in case of error
     * @return AEROSPIKE_OK if success. Otherwise AEROSPIKE_ERR_*.
     *******************************************************************************************
     */
    static as_status as_key_to_php_key(const as_key *key_p, Array& php_key, as_error& error)
    {
        as_error_reset(&error);
        if (!key_p) {
            return as_error_update(&error, AEROSPIKE_ERR_CLIENT,
                    "Record key is null");
        }

        if (key_p->ns && strlen(key_p->ns) > 0) {
            php_key.set(s_ns, String(key_p->ns));
        }

        if (key_p->set && strlen(key_p->set) > 0) {
            php_key.set(s_set, String(key_p->set));
        }

        if (key_p->valuep) {
            as_val *value_p = (as_val *) key_p->valuep;
            switch (as_val_type(value_p)) {
                case AS_INTEGER:
                    {
                        as_integer *ivalue_p = as_integer_fromval(value_p);
                        php_key.set(s_key, as_integer_get(ivalue_p));
                        break;
                    }
                case AS_STRING:
                    {
                        as_string *svalue_p = as_string_fromval(value_p);
                        php_key.set(s_key, as_string_get(svalue_p));
                        break;
                    }
                default:
                    {
                        as_error_update(&error, AEROSPIKE_ERR_CLIENT,
                                "Invalid key type");
                    }
            }
        }

        if (key_p->digest.init) {
            php_key.set(s_digest, String((char *) key_p->digest.value),
                    AS_DIGEST_VALUE_SIZE);
        }

        return error.code;
    }

    /*
     *******************************************************************************************
     * Function to convert as_record into PHP record Array
     *
     * @param record_p      as_record to be converted by this function
     * @param key_p         as_key to be used to populate PHP key within PHP
     *                      record array
     * @param php_record    PHP variant reference to be populated by this
     *                      function
     * @param error         as_error reference to be populated by this function
     *                      in case of error
     * @return AEROSPIKE_OK if success. Otherwise AEROSPIKE_ERR_*.
     *******************************************************************************************
     */
    as_status as_record_to_php_record(const as_record *record_p, const as_key *key_p, VRefParam php_record, as_error& error)
    {
        as_error_reset(&error);
        if (!record_p) {
            return as_error_update(&error, AEROSPIKE_ERR_CLIENT,
                    "Record is null");
        }

        Array temp_php_record = Array::Create();
        Array php_key = Array::Create();
        Array php_metadata = Array::Create();
        Array php_bins = Array::Create();

        as_key_to_php_key(key_p ? key_p : &record_p->key, php_key, error);
        metadata_to_php_metadata(record_p, php_metadata, error);
        bins_to_php_bins(record_p, php_bins, error);

        temp_php_record.set(s_key, php_key);
        temp_php_record.set(s_metadata, php_metadata);
        temp_php_record.set(s_bins, php_bins);
        php_record = temp_php_record;

        return error.code;
    }
} // namespace HPHP

