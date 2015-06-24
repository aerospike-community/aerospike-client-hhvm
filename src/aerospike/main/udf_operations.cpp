#include "udf_operations.h"
#include "ext_aerospike.h"

namespace HPHP {
    /*
     *******************************************************************************************
     * Function to read UDF contents from the specified file
     *
     * @param as_P                  The aerospike pointer for the current
     *                              opeartion
     * @param lua_path              The path to the lua file
     * @param lua_module            The lua module to be registered with Aeropsike
     *                              cluster
     * @param udf_content_p         The UDF contents to be writtenn to the DB
     * @param error                 as_error reference to be populated by this function
     *                              in case of error
     *
     * @return AEROSPIKE_OK if success. Otherwise AEROSPIKE_ERR_*.
     *******************************************************************************************
     */
    as_status read_udf_file_contents(aerospike *as_p, const Variant& lua_path, as_bytes* udf_content_p, as_error& error)
    {
        FILE *udf_file_path = NULL;
        uint8_t* bytes_p = NULL;
        uint8_t* buff_p = NULL;

        uint32_t size = 0;
        uint32_t content_size;
        uint32_t read;
        
        as_error_reset(&error);
        
        const char* file_path = lua_path.toString().c_str();
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
            }

            const char *filename = strrchr(file_path, '/');
            if(!filename) {
                filename = file_path;
            } else if(filename[0] == '/') {
                filename = filename + 1;
            }

            memcpy(copy_filepath + user_path_len + 1, filename, strlen(filename));

            FILE *fileW_p = NULL;
            fileW_p = fopen(copy_filepath, "r");
            if(!fileW_p && errno == ENOENT) {
                fileW_p = fopen(copy_filepath, "w");
                if (!fileW_p) {
                    return as_error_update(&error, AEROSPIKE_ERR_UDF_NOT_FOUND, "Cannot create script file");
                }

                if (0 >= (int) fwrite(bytes_p, size, 1, fileW_p)) {
                    return as_error_update(&error, AEROSPIKE_ERR_UDF_NOT_FOUND, "unable to write to script file");
                }
            }

            as_bytes_init_wrap(udf_content_p, bytes_p, size, false);

            if (udf_file_path) {
                fclose(udf_file_path);
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
     * @param lua_path              The path to the lua file
     * @param lua_module            The lua module to be registered with Aeropsike
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
    as_status register_udf_module(aerospike *as_p, const Variant& lua_path, const Variant& lua_module,
            const Variant& language, as_policy_info *info_policy_p, as_error& error)
    {
        as_bytes udf_content;
        as_bytes* udf_content_p = NULL;

        as_error_reset(&error);

        if (!lua_path.isString() || !lua_module.isString()) {
            as_error_update(&error, AEROSPIKE_ERR_PARAM, "Lua path/Lua module should be of type string");
        } else if (!language.isInteger()) {
            as_error_update(&error, AEROSPIKE_ERR_PARAM, "Invalid language type");
        } else {
            udf_content_p = &udf_content;
            if (AEROSPIKE_OK == read_udf_file_contents(as_p, lua_path, &udf_content, error)) {
                const char* module_p = lua_module.toString().c_str();
                
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
     * @param lua_module            The lua module to be registered with Aeropsike
     *                              cluster
     * @param info_policy_p         The as_policy_info to be used for this
     *                              operation
     * @param error                 as_error reference to be populated by this function
     *                              in case of error
     *
     * @return AEROSPIKE_OK if success. Otherwise AEROSPIKE_ERR_*.
     *******************************************************************************************
     */
    as_status remove_udf_module(aerospike *as_p, const Variant& lua_module, as_policy_info *info_policy_p, as_error& error)
    {
        as_error_reset(&error);

        if (lua_module.isString()) {
            const char* module_p = lua_module.toString().c_str();
            aerospike_udf_remove(as_p, &error, info_policy_p, module_p);
        } else {
            as_error_update(&error, AEROSPIKE_ERR_PARAM, "Invalid lua module type");
        }
        return error.code;
    }
}
