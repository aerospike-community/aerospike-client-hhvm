#ifndef __EXT_AEROSPIKE_H__
#define __EXT_AEROSPIKE_H__

extern "C" {
#include "aerospike/aerospike.h"
#include "aerospike/as_config.h"
#include "aerospike/aerospike_key.h"
#include "aerospike/as_status.h"
#include "aerospike/aerospike_scan.h"
#include "pthread.h"
#include "stdlib.h"
}

#include "constants.h"

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
    const StaticString s_progress_pct("progress_pct");
    const StaticString s_records_scanned("records_scanned");
    const StaticString s_status("status");

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
            as_error latest_error;
            pthread_rwlock_t latest_error_mutex;

            Aerospike();
            void sweep();
            ~Aerospike();

            as_status configure_connection(as_config& config, as_error& error);

        private:
            void create_new_host_entry(as_config& config, as_error& error);
            void iterate_hosts_add_entry(as_config& config, int matched_host_id);
    };

    void HHVM_METHOD(Aerospike, __construct, const Array& config,
            bool persistent_connection, const Variant& options);
    bool HHVM_METHOD(Aerospike, isConnected);
    int64_t HHVM_METHOD(Aerospike, close);
    int64_t HHVM_METHOD(Aerospike, reconnect);
    int64_t HHVM_METHOD(Aerospike, put, const Array& php_key,
            const Array& php_rec, int64_t ttl, const Variant& options);
    int64_t HHVM_METHOD(Aerospike, get, const Array& php_key,
            VRefParam php_rec, const Variant& filter_bins,
            const Variant& options);
    int64_t HHVM_METHOD(Aerospike, getMany, const Array& php_keys,
            VRefParam php_records, const Variant& filter_bins,
            const Variant& options);
    int64_t HHVM_METHOD(Aerospike, operate, const Array& php_key,
            const Array& operations, VRefParam returned,
            const Variant& options);
    int64_t HHVM_METHOD(Aerospike, remove, const Array& php_key,
            const Variant& options);
    int64_t HHVM_METHOD(Aerospike, removeBin, const Array& php_key,
            const Array& bins, const Variant& options);
    int64_t HHVM_METHOD(Aerospike, exists, const Array& php_key,
            VRefParam metadata, const Variant& options);
    int64_t HHVM_METHOD(Aerospike, existsMany, const Array& php_keys,
            VRefParam metadata, const Variant& options);
    String HHVM_METHOD(Aerospike, getKeyDigest, const Variant& ns,
            const Variant& set, const Variant& primary_key);
    int64_t HHVM_METHOD(Aerospike, errorno);
    String HHVM_METHOD(Aerospike, error);

} // namespace HPHP
#endif /* end of __EXT_AEROSPIKE_H__ */
