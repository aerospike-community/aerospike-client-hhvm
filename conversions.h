#include "hphp/runtime/base/base-includes.h"
#include "hphp/runtime/base/execution-context.h"
#include "hphp/runtime/vm/native-data.h"

extern "C" {
#include "aerospike/aerospike_key.h"
#include "aerospike/as_status.h"
#include "aerospike/as_record.h"
}

namespace HPHP {
    extern as_status php_config_to_as_config(Array php_config, as_config *config_p);
    extern as_status php_key_to_as_key(Array php_key, as_key *key_p);
    extern as_status php_record_to_as_record(Array php_rec, as_record *rec_p);
    extern as_status as_record_to_php_record(VRefParam php_rec, as_record *rec_p);
} // namespace HPHP

