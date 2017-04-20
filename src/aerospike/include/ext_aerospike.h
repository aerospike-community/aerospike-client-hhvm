#ifndef __EXT_AEROSPIKE_H__
#define __EXT_AEROSPIKE_H__

extern "C" {
#include "aerospike/aerospike.h"
#include "aerospike/as_config.h"
#include "aerospike/aerospike_key.h"
#include "aerospike/as_status.h"
#include "aerospike/aerospike_scan.h"
#include "aerospike/aerospike_query.h"
#include "pthread.h"
#include "stdlib.h"
}

#include "constants.h"
#include "hphp/runtime/ext/extension.h"
#include "hphp/runtime/base/request-local.h"

namespace HPHP {
#define MAX_PORT_SIZE 6

    /*
     *******************************************************************************************************
     * Structure containing C client's aerospike object and its reference counter.
     *******************************************************************************************************
     */
    typedef struct csdk_aerospike_object {
        /*
         * as_p holds the reference of internal C SDK aerospike object
         */
        aerospike *as_p;

        /*
         * ref_as_p indicates the no. of references for internal C
         * SDK aerospike object being held by the various PHP userland Aerospike
         * objects.
         */
        int ref_php_object;

        /*
         * ref_hosts_entry indicates the no. of references for internal C
         * SDK aerospike object being held by entries in aerospike global
         * persistent_list hashtable.
         */
        int ref_host_entry;
    } aerospike_ref;

    /*
     ************************************************************************************
     * Constant Static Strings used throughout the extension code.
     ************************************************************************************
     */
    const StaticString s_Aerospike("Aerospike");
    const StaticString s_hosts("hosts");
    const StaticString s_addr("addr");
    const StaticString s_port("port");
    const StaticString s_user("user");
    const StaticString s_pass("pass");
    const StaticString s_ns("ns");
    const StaticString s_set("set");
    const StaticString s_key("key");
    const StaticString s_digest("digest");
    const StaticString s_metadata("metadata");
    const StaticString s_ttl("ttl");
    const StaticString s_generation("generation");
    const StaticString s_bins("bins");
    const StaticString s_op("op");
    const StaticString s_bin("bin");
    const StaticString s_val("val");
    const StaticString s_index_type("index_type");
    const StaticString s_progress_pct("progress_pct");
    const StaticString s_records_scanned("records_scanned");
    const StaticString s_status("status");
    const StaticString s_udf_module_name("name");
    const StaticString s_udf_module_type("type");
    const StaticString s_shm("shm");
    const StaticString s_shm_key("shm_key");
    const StaticString s_shm_max_nodes("shm_max_nodes");
    const StaticString s_shm_max_namespaces("shm_max_namespaces");
    const StaticString s_shm_takeover_threshold_sec("shm_takeover_threshold_sec");
    
    /* Request-local globals for serializer/deserializer */
    struct AerospikeRequestLocals : RequestEventHandler {
        Variant serializer, deserializer;
        void vscan (IMarker &mark) const override {}
        void requestInit() override {}
        void requestShutdown() override {
            serializer = null_variant;
            deserializer = null_variant;
        }
    };

    /*
     ************************************************************************************
     * Aerospike class to maintain the following data:
     * 1. Aerospike C client's connection object
     * 2. latest instance level error
     * 3. is_connected flag
     * 4. ref_count for the Aerospike C client's connection object reference
     ************************************************************************************
     */
    class Aerospike {
        public:
            aerospike_ref *as_ref_p{nullptr};
            bool is_connected = false;
            bool is_persistent = false;
            int16_t serializer_value = SERIALIZER_PHP;

            Aerospike();
            void sweep();
            ~Aerospike();

            static bool hasSerializer() {
                return locals.getInited() && !locals->serializer.isNull();
            }
            static void setSerializer(const Variant& callback) {
                locals->serializer = callback;
            }
            static Variant serializer() {
                if (locals.getInited()) {
                    return locals->serializer;
                }
                return null_variant;
            }
            static bool hasDeserializer() {
                return locals.getInited() && !locals->deserializer.isNull();
            }
            static void setDeserializer(const Variant& callback) {
                locals->deserializer = callback;
            }
            static Variant deserializer() {
                if (locals.getInited()) {
                    return locals->deserializer;
                }
                return null_variant;
            }

            as_status configure_connection(as_config& config, as_error& error);

            void setError(const as_error& error);
            const as_error& getError() { return latest_error; }

        private:
            DECLARE_STATIC_REQUEST_LOCAL(AerospikeRequestLocals, locals);
            as_error latest_error;
            pthread_rwlock_t latest_error_mutex;

            void create_new_host_entry(as_config& config, as_error& error);
            void iterate_hosts_add_entry(as_config& config, int matched_host_id);
    };

} // namespace HPHP
#endif /* end of __EXT_AEROSPIKE_H__ */
