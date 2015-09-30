#ifndef __CONSTANTS_H__
#define __CONSTANTS_H__

extern "C" {
#include "aerospike/as_status.h"
#include "aerospike/as_policy.h"
#include "aerospike/as_scan.h"
#include "aerospike/as_udf.h"
#include "aerospike/as_error.h"
#include "aerospike/aerospike_index.h"
#include "aerospike/as_operations.h"
}
namespace HPHP {
    #define MAX_SIZE_OF_CONSTANT_NAME 512

    /*
     *******************************************************************************************************
     * Enum for PHP client's optional policy constant keys. (OPT_*)
     *******************************************************************************************************
     */
    enum Aerospike_constants {
        OPT_CONNECT_TIMEOUT = 1,  /* value in milliseconds, default: 1000 */
        OPT_READ_TIMEOUT,         /* value in milliseconds, default: 1000 */
        OPT_WRITE_TIMEOUT,        /* value in milliseconds, default: 1000 */
        OPT_POLICY_RETRY,         /* set to a Aerospike::POLICY_RETRY_* value */
        OPT_POLICY_EXISTS,        /* set to a Aerospike::POLICY_EXISTS_* value */
        OPT_SERIALIZER,           /* set the unsupported type handler */
        OPT_SCAN_PRIORITY,        /* set to a Aerospike::SCAN_PRIORITY_* value*/
        OPT_SCAN_PERCENTAGE,      /* integer value 1-100, default: 100 */
        OPT_SCAN_CONCURRENTLY,    /* boolean value, default: false */
        OPT_SCAN_NOBINS,          /* boolean value, default: false */
        OPT_POLICY_KEY,           /* records store the digest unique ID, optionally also its (ns,set,key) inputs */
        OPT_POLICY_GEN,
        OPT_POLICY_REPLICA,       /* set to one of Aerospike::POLICY_REPLICA_* */
        OPT_POLICY_CONSISTENCY,   /* set to one of Aerospike::POLICY_CONSISTENCY_* */
        OPT_POLICY_COMMIT_LEVEL,  /* set to one of Aerospike::POLICY_COMMIT_LEVEL_* */
        OPT_TTL                   /* set to time-to-live of the record in seconds */
    };

    /*
     *******************************************************************************************************
     * Enum for PHP client's SERIALIZER_* constant values. Possible values for
     * OPT_SERIALIZER.
     *******************************************************************************************************
     */
    enum Aerospike_serializer_values {
        SERIALIZER_NONE,
        SERIALIZER_PHP,                                     /* default handler for serializer type */
        SERIALIZER_JSON,
        SERIALIZER_USER,
    };

    #define SERIALIZER_DEFAULT "1"

    /* 
     *******************************************************************************************************
     * Structure to map constant number to constant name string for Aerospike extension constants.
     *******************************************************************************************************
     */
    typedef struct __aerospike_constants {
        int constant_no;
        char constant_name[MAX_SIZE_OF_CONSTANT_NAME];
    } aerospike_constants;
} // namespace HPHP
#endif /* end of __CONSTANTS_H__ */
