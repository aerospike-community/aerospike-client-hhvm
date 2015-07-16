#include "udf_operations.h"
#include "ext_aerospike.h"
#include "conversions.h"

namespace HPHP {
    /*
     *******************************************************************************************
     * Function to read UDF contents from the specified file
     *
     * @param as_P                  The aerospike pointer for the current
     *                              opeartion
     * @param path                  The path to the UDF module
     * @param module                The UDF module to be registered with Aeropsike
     *                              cluster
     * @param udf_content_p         The UDF contents to be writtenn to the DB
     * @param error                 as_error reference to be populated by this function
     *                              in case of error
     *
     * @return AEROSPIKE_OK if success. Otherwise AEROSPIKE_ERR_*.
     *******************************************************************************************
     */
    as_status read_udf_file_contents(aerospike *as_p, const Variant& path, as_bytes* udf_content_p, as_error& error)
    {
        FILE *udf_file_path = NULL;
        FILE *fileW_p = NULL;
        uint8_t* bytes_p = NULL;
        uint8_t* buff_p = NULL;

        uint32_t size = 0;
        uint32_t content_size;
        uint32_t read;
        
        as_error_reset(&error);
        
        const char* file_path = path.toString().c_str();
        udf_file_path = fopen(file_path, "r");
        if (!udf_file_path) {
            as_error_update(&error, AEROSPIKE_ERR_UDF_NOT_FOUND,
                    "Cannot open script");
        } else {
            fseek(udf_file_path, 0L, SEEK_END);
            content_size = ftell(udf_file_path);
            fseek(udf_file_path, 0L, SEEK_SET);

            if (NULL == (bytes_p = (uint8_t *) malloc(content_size + 1))) {
                return as_error_update(&error, AEROSPIKE_ERR_CLIENT,
                        "Memory allocation failed for UDF contents");
            }

            buff_p = bytes_p;

            read = (int) fread(buff_p, 1, LUA_FILE_BUFFER_FRAME, udf_file_path);
            while (read) {
                size += read;
                buff_p += read;
                read = (int) fread(buff_p, 1, LUA_FILE_BUFFER_FRAME, udf_file_path);
            }
       
            const char *udf_path = as_p->config.lua.user_path;
            char copy_filepath[AS_CONFIG_PATH_MAX_LEN] = {0};
            uint32_t user_path_len = strlen(udf_path);

            memcpy(copy_filepath, udf_path, user_path_len);

            if(udf_path[user_path_len - 1] != '/') {
                memcpy(copy_filepath + user_path_len, "/", 1);
                user_path_len++;
            }

            const char *filename = strrchr(file_path, '/');
            if(!filename) {
                filename = file_path;
            } else if(filename[0] == '/') {
                filename = filename + 1;
            }

            memcpy(copy_filepath + user_path_len, filename, strlen(filename));
            fileW_p = fopen(copy_filepath, "w");
            if(!fileW_p) {
                if (errno == ENOENT) {
                    return as_error_update(&error, AEROSPIKE_ERR_UDF_NOT_FOUND,
                            "Cannot create script file due to no such directory");
                } else if (errno == EACCES) {
                    return as_error_update(&error, AEROSPIKE_ERR_UDF_NOT_FOUND,
                            "Cannot create script file due to invalid permissions");
                } else {
                    return as_error_update(&error, AEROSPIKE_ERR_UDF_NOT_FOUND, "Cannot create script file");
                }
            }

            if (0 >= (int) fwrite(bytes_p, size, 1, fileW_p)) {
                return as_error_update(&error, AEROSPIKE_ERR_UDF_NOT_FOUND, "unable to write to script file");
            }

            as_bytes_init_wrap(udf_content_p, bytes_p, size, false);

            if (udf_file_path) {
                fclose(udf_file_path);
            }

            if (fileW_p) {
                fclose(fileW_p);
            }

            if (bytes_p) {
                free(bytes_p);
            }
        }

        return error.code;
    }

    /*
     *******************************************************************************************
     * Function to register a udf module with the Aerospike cluster
     *
     * @param as_p                  Aerospike pointer to be used by this operation
     * @param path                  The path to the lua file
     * @param module                The lua module to be registered with Aeropsike
     *                              cluster
     * @param language              The UDF type to be registered
     * @param info_policy_p         The as_policy_info to be used for this
     *                              operation
     * @param error                 as_error reference to be populated by this function
     *                              in case of error
     *
     * @return AEROSPIKE_OK if success. Otherwise AEROSPIKE_ERR_*.
     *******************************************************************************************
     */
    as_status register_udf_module(aerospike *as_p, const Variant& path, const Variant& module,
            const Variant& language, as_policy_info *info_policy_p, as_error& error)
    {
        as_bytes udf_content;
        as_bytes* udf_content_p = NULL;

        as_error_reset(&error);

        if (!path.isString() || !module.isString()) {
            as_error_update(&error, AEROSPIKE_ERR_PARAM, "UDF path/module should be of type string");
        } else if (path.toString().length() == 0 || module.toString().length() == 0) {
            as_error_update(&error, AEROSPIKE_ERR_PARAM, "UDF path/module should not be empty string");
        } else if (!language.isInteger()) {
            as_error_update(&error, AEROSPIKE_ERR_PARAM, "Invalid language type");
        } else {
            udf_content_p = &udf_content;
            if (AEROSPIKE_OK == read_udf_file_contents(as_p, path, &udf_content, error)) {
                const char* module_p = module.toString().c_str();
                
                if (AEROSPIKE_OK == aerospike_udf_put(as_p, &error, info_policy_p, module_p,
                            (as_udf_type) language.toInt32(), udf_content_p)) {
                    if (AEROSPIKE_OK == aerospike_udf_put_wait(as_p, &error, info_policy_p, module_p, 0)) {
                    }
                }
            }

            if (udf_content_p) {
                as_bytes_destroy(&udf_content);
            }
        }

        return error.code;
    }
    
    /*
     *******************************************************************************************
     * Function to remove a udf module from the Aerospike cluster
     *
     * @param as_p                  Aerospike pointer to be used by this operation
     * @param module                The lua module to be registered with Aeropsike
     *                              cluster
     * @param info_policy_p         The as_policy_info to be used for this
     *                              operation
     * @param error                 as_error reference to be populated by this function
     *                              in case of error
     *
     * @return AEROSPIKE_OK if success. Otherwise AEROSPIKE_ERR_*.
     *******************************************************************************************
     */
    as_status remove_udf_module(aerospike *as_p, const Variant& module, as_policy_info *info_policy_p, as_error& error)
    {
        as_error_reset(&error);

        if (module.isString()) {
            if (module.toString().length() == 0) {
                as_error_update(&error, AEROSPIKE_ERR_PARAM, "UDF module should not be empty string");
            } else {
                const char* module_p = module.toString().c_str();
                aerospike_udf_remove(as_p, &error, info_policy_p, module_p);
            }
        } else {
            as_error_update(&error, AEROSPIKE_ERR_PARAM, "Invalid lua module type");
        }

        return error.code;
    }
    /*
     *******************************************************************************************
     * Function to apply a UDF function
     *
     * @param as_p                  Aerospike pointer to be used by this operation
     * @param key                   Key of the record the function to be applied to
     * @param module                The lua module to be registered with Aeropsike
     *                              cluster
     * @param function              The UDF lua function to be applied on the record
     * @param args                  The arguments to the LUA function
     * @param apply_policy_p        The as_policy_apply to be used for this
     *                              operation
     * @param static_pool           StaticPoolManager instance reference, to be used for
     *                              the conversion lifecycle.
     * @param serializer_type       The serializer_policy to be used to handle
     * @param returned_type         The as_val to be returned
     * @param error                 as_error reference to be populated by this function
     *                              in case of error
     *
     * @return AEROSPIKE_OK if success. Otherwise AEROSPIKE_ERR_*.
     *******************************************************************************************
     */
    as_status aerospike_udf_apply(aerospike *as_p, as_key key, const Variant& module, const Variant& function, const Variant& args,
            as_policy_apply *apply_policy_p, StaticPoolManager& static_pool, int16_t serializer_type, VRefParam php_returned_value,
            as_error& error)
    {
        Array php_udf_args;
        as_val* udf_result = NULL;
        as_list *args_list = NULL;

        as_error_reset(&error);

        if (!module.isString() || !function.isString() || module.toString().empty()
                || function.toString().empty()) {
            as_error_update(&error, AEROSPIKE_ERR_PARAM,
                    "Lua module/Lua function should be of type string and non-empty");
        } 
        if (!args.isArray()) {
            as_error_update(&error, AEROSPIKE_ERR_PARAM, "Lua function arguments should be of type array");
        }
        php_udf_args = args.toArray();
        if (!args.isNull() && (AEROSPIKE_OK != php_list_to_as_list(php_udf_args, &args_list, static_pool,
                    serializer_type, error))) {
            //Argument list creation for UDF failed
            return error.code;
        }
        const char* module_p = module.toString().c_str();
        const char* function_p = function.toString().c_str();

        if( AEROSPIKE_OK == aerospike_key_apply(as_p, &error, apply_policy_p, &key, module_p,
                    function_p, args_list, &udf_result))
        {
            as_val_to_php_variant(udf_result, php_returned_value, error);
        }

        return error.code;
    }
    /*
     *******************************************************************************************
     * Function to get the code of UDF module which is registered with the server
     *
     * @param as_p                  Aerospike pointer to be used by this operation
     * @param module                The lua module to be registered with Aeropsike
     *                              cluster
     * @param module_code           The contents of the UDF module
     * @param language              The UDF type to be registered
     * @param info_policy_p         The as_policy_info to be used for this
     *                              operation
     * @param error                 as_error reference to be populated by this function
     *                              in case of error
     *
     * @return AEROSPIKE_OK if success. Otherwise AEROSPIKE_ERR_*.
     *******************************************************************************************
     */
    as_status get_registered_udf_module_code(aerospike *as_p, const Variant& module, String &module_code, const Variant& language,
            as_policy_info *info_policy_p, as_error& error)
    {
        as_error_reset(&error);

        if (module.isString()) {
            if (module.toString().length() == 0) {
                as_error_update(&error, AEROSPIKE_ERR_PARAM, "UDF module should not be empty string");
            } else {
                if (language.isInteger()) {
                    as_udf_file udf_file;
                    const char *module_name_p = module.toString().c_str();

                    as_udf_file_init(&udf_file);

                    if (AEROSPIKE_OK == aerospike_udf_get(as_p, &error, info_policy_p, module_name_p,
                                (as_udf_type) language.toInt32(), &udf_file)) {
                        char *udf_content = (char *)malloc(udf_file.content.size + 1);
                        if (udf_content) {
                            memset(udf_content, 0, udf_file.content.size + 1);
                            memcpy(udf_content, (char *)udf_file.content.bytes, udf_file.content.size);
                            module_code = String((char *)udf_content);
                            free(udf_content);
                        } else {
                            as_error_update(&error, AEROSPIKE_ERR_PARAM, "UDF file content copying failed");
                        }
                    }
                    as_udf_file_destroy(&udf_file);
                } else {
                    as_error_update(&error, AEROSPIKE_ERR_PARAM, "Invalid language type");
                }
            }
        } else {
            as_error_update(&error, AEROSPIKE_ERR_PARAM, "Invalid module type");
        }

        return error.code;
    }

    /*
     *******************************************************************************************
     * Function to list the UDF modules registered with the server
     *
     * @param as_p                  Aerospike pointer to be used by this operation
     * @param modules               The UDF modules registered with Aeropsike
     *                              cluster
     * @param language              The UDF type to be registered
     * @param info_policy_p         The as_policy_info to be used for this
     *                              operation
     * @param error                 as_error reference to be populated by this function
     *                              in case of error
     *
     * @return AEROSPIKE_OK if success. Otherwise AEROSPIKE_ERR_*.
     *******************************************************************************************
     */
    as_status list_registered_udf_modules(aerospike *as_p, Array& modules, const Variant& language,
            as_policy_info *info_policy_p, as_error& error)
    {
        as_udf_files udf_files;

        as_error_reset(&error);

        if (language.isInteger()) {
            as_udf_files_init(&udf_files, 0);
            if (AEROSPIKE_OK == aerospike_udf_list(as_p, &error, info_policy_p, &udf_files)) {
                for (int i = 0; i < udf_files.size; i++) {
                    if (language.toInt32() != udf_files.entries[i].type) {
                        continue;
                    }
                    as_udf_file *file = &udf_files.entries[i];
                    Array file_entry = Array::Create();
                    file_entry.set(s_udf_module_name, file->name);
                    file_entry.set(s_udf_module_type, file->type);
                    modules.append(file_entry);
                }
            }
        } else {
            as_error_update(&error, AEROSPIKE_ERR_PARAM, "Invalid module type");
        }

        return error.code;
    }
}
