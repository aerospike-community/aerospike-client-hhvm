extern "C" {
#include "aerospike/aerospike.h"
#include "aerospike/as_config.h"
#include "aerospike/aerospike_key.h"
#include "aerospike/as_status.h"
}

namespace HPHP {
    /*
     ************************************************************************************
     * Constant Static Strings used throughout the extension code.
     ************************************************************************************
     */
    const StaticString s_Aerospike("Aerospike");
    const StaticString s_hosts("hosts");
    const StaticString s_addr("addr");
    const StaticString s_port("port");
    const StaticString s_ns("ns");
    const StaticString s_set("set");
    const StaticString s_key("key");
    const StaticString s_digest("digest");
    const StaticString s_metadata("metadata");
    const StaticString s_ttl("ttl");
    const StaticString s_generation("generation");
    const StaticString s_bins("bins");

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
            aerospike *as_p{nullptr};
            as_error latest_error;
            bool is_connected = false;
            int ref_count = 0;

            Aerospike();
            void sweep();
            ~Aerospike();
    };

    void HHVM_METHOD(Aerospike, __construct, const Array& config);
    bool HHVM_METHOD(Aerospike, isConnected);
    int HHVM_METHOD(Aerospike, close);
    int HHVM_METHOD(Aerospike, put, const Array& php_key, const Array& php_rec);
    int HHVM_METHOD(Aerospike, get, const Array& php_key, VRefParam php_rec);
    int HHVM_METHOD(Aerospike, errorno);
    String HHVM_METHOD(Aerospike, error);

} // namespace HPHP

