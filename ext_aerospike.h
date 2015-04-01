extern "C" {
#include "aerospike/aerospike.h"
#include "aerospike/as_config.h"
#include "aerospike/aerospike_key.h"
#include "aerospike/as_status.h"
}

namespace HPHP {
    const StaticString s_Aerospike("Aerospike");
    const StaticString s_hosts("hosts");
    const StaticString s_addr("addr");
    const StaticString s_port("port");
    const StaticString s_ns("ns");
    const StaticString s_set("set");
    const StaticString s_key("key");

    class Aerospike {
        public:
            aerospike *as_p{nullptr};
            as_error latest_error;
            bool is_connected = false;
            int ref_count = 0;

            Aerospike() { }
            ~Aerospike() {
                sweep();
            }
            void sweep() {
                as_error error;
                if (as_p) {
                    if (0 != ref_count) {
                        aerospike_close(as_p, &error);
                        ref_count = 0;
                    }
                    aerospike_destroy(as_p);
                    as_p = nullptr;
                    is_connected = false;
                }
            }
    };

    void HHVM_METHOD(Aerospike, __construct, const Array& config);
    bool HHVM_METHOD(Aerospike, isConnected);
    int HHVM_METHOD(Aerospike, close);
    int HHVM_METHOD(Aerospike, put, const Array& php_key, const Array& php_rec);
    int HHVM_METHOD(Aerospike, get, const Array& php_key, VRefParam php_rec);
    int HHVM_METHOD(Aerospike, errorno);
    String HHVM_METHOD(Aerospike, error);

} // namespace HPHP

