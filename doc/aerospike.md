
# The Aerospike class
The Aerospike HHVM client API may be described as follows:

## Introduction

The main Aerospike class

```php

class Aerospike
{
    // The key policy can be determined by setting OPT_POLICY_KEY to one of
    const POLICY_KEY_DIGEST; // hashes (ns,set,key) data into a unique record ID (default)
    const POLICY_KEY_SEND;   // also send, store, and get the actual (ns,set,key) with each record

    // The generation policy can be set using OPT_POLICY_GEN to one of
    const POLICY_GEN_IGNORE; // write a record, regardless of generation
    const POLICY_GEN_EQ;     // write a record, ONLY if given value is equal to the current record generation
    const POLICY_GEN_GT;     // write a record, ONLY if given value is greater-than the current record generation

    // The retry policy can be determined by setting OPT_POLICY_RETRY to one of
    const POLICY_RETRY_NONE; // do not retry an operation (default)
    const POLICY_RETRY_ONCE; // allow for a single retry on an operation

    // By default writes will try to create or replace records and bins
    // behaving similar to an array in PHP. Setting
    // OPT_POLICY_EXISTS with one of these values will overwrite this.
    // POLICY_EXISTS_IGNORE (aka CREATE_OR_UPDATE) is the default value
    const POLICY_EXISTS_IGNORE;            // interleave bins of a record if it exists
    const POLICY_EXISTS_CREATE;            // create a record ONLY if it DOES NOT exist
    const POLICY_EXISTS_UPDATE;            // update a record ONLY if it exists
    const POLICY_EXISTS_REPLACE;           // replace a record ONLY if it exists
    const POLICY_EXISTS_CREATE_OR_REPLACE; // overwrite the bins if record exists

    // Replica and consistency guarantee options
    // See: http://www.aerospike.com/docs/client/c/usage/consistency.html
    const POLICY_REPLICA_MASTER;      // read from the partition master replica node (default)
    const POLICY_REPLICA_ANY;         // read from either the master or prole node
    const POLICY_CONSISTENCY_ONE;     // involve a single replica in the read operation (default)
    const POLICY_CONSISTENCY_ALL;     // involve all replicas in the read operation
    const POLICY_COMMIT_LEVEL_ALL;    // return success after committing all replicas (default)
    const POLICY_COMMIT_LEVEL_MASTER; // return success after committing the master replica

    // Options can be assigned values that modify default behavior
    const OPT_CONNECT_TIMEOUT;    // value in milliseconds, default: 1000
    const OPT_READ_TIMEOUT;       // value in milliseconds, default: 1000
    const OPT_WRITE_TIMEOUT;      // value in milliseconds, default: 1000
    const OPT_POLICY_RETRY;       // set to a Aerospike::POLICY_RETRY_* value
    const OPT_POLICY_EXISTS;      // set to a Aerospike::POLICY_EXISTS_* value
    const OPT_POLICY_KEY;         // records store the digest unique ID, optionally also its (ns,set,key) inputs
    const OPT_POLICY_GEN;         // set to array( Aerospike::POLICY_GEN_* [, $gen_value ] )
    const OPT_POLICY_REPLICA;     // set to one of Aerospike::POLICY_REPLICA_*
    const OPT_POLICY_CONSISTENCY; // set to one of Aerospike::POLICY_CONSISTENCY_*
    const OPT_POLICY_COMMIT_LEVEL;// set to one of Aerospike::POLICY_COMMIT_LEVEL_*

    // Aerospike Status Codes:
    //
    // Each Aerospike API method invocation returns a status code
    //  depending upon the success or failure condition of the call.
    //
    // The error status codes map to the C client
    //  src/include/aerospike/as_status.h

    // Client status codes:
    //
    const ERR_PARAM              ; // Invalid client parameter
    const ERR_CLIENT             ; // Generic client error

    // Server status codes:
    //
    const OK                     ; // Success status
    const ERR_SERVER             ; // Generic server error
    const ERR_SERVER_FULL        ; // Node running out of memory/storage
    const ERR_DEVICE_OVERLOAD    ; // Node storage lagging write load
    const ERR_TIMEOUT            ; // Client or server side timeout error
    const ERR_CLUSTER            ; // Generic cluster discovery and connection error
    const ERR_CLUSTER_CHANGE     ; // Cluster state changed during the request
    const ERR_REQUEST_INVALID    ; // Invalid request protocol or protocol field
    const ERR_UNSUPPORTED_FEATURE;
    const ERR_NO_XDR             ; // XDR not available for the cluster
    // Record specific:
    const ERR_NAMESPACE_NOT_FOUND;
    const ERR_RECORD_NOT_FOUND   ;
    const ERR_RECORD_EXISTS      ; // Record already exists
    const ERR_RECORD_GENERATION  ; // Write policy regarding generation violated
    const ERR_RECORD_TOO_BIG     ; // Record written cannot fit in storage write block
    const ERR_RECORD_BUSY        ; // Hot key: too many concurrent requests for the record
    const ERR_RECORD_KEY_MISMATCH; // Digest incompatibility?
    // Bin specific:
    const ERR_BIN_NAME           ; // Name too long or exceeds the unique name quota for the namespace
    const ERR_BIN_NOT_FOUND      ;
    const ERR_BIN_EXISTS         ; // Bin already exists
    const ERR_BIN_INCOMPATIBLE_TYPE;

    // Logger
    const LOG_LEVEL_OFF  ;
    const LOG_LEVEL_ERROR;
    const LOG_LEVEL_WARN ;
    const LOG_LEVEL_INFO ;
    const LOG_LEVEL_DEBUG;
    const LOG_LEVEL_TRACE;

    // Multi-operation operators map to the C client
    //  src/include/aerospike/as_operations.h
    const OPERATOR_WRITE;
    const OPERATOR_READ;
    const OPERATOR_INCR;
    const OPERATOR_PREPEND;
    const OPERATOR_APPEND;
    const OPERATOR_TOUCH;

    // lifecycle and connection methods
    public __construct ( array $config [,  boolean $persistent_connection = true [, array $options]] )
    public __destruct ( void )
    public boolean isConnected ( void )
    public close ( void )
    public reconnect ( void )

    // error handling methods
    public string error ( void )
    public int errorno ( void )
    // TBD
    public setLogLevel ( int $log_level )
    public setLogHandler ( callback $log_handler )

    // key-value methods
    public array initKey ( string $ns, string $set, int|string $pk [, boolean $is_digest = false ] )
    public string getKeyDigest ( string $ns, string $set, int|string $pk )
    public int put ( array $key, array $bins [, int $ttl = 0 [, array $options ]] )
    public int get ( array $key, array &$record [, array $filter [, array $options ]] )
    public int exists ( array $key, array &$metadata [, array $options ] )
    public int touch ( array $key, int $ttl = 0 [, array $options ] )
    public int remove ( array $key [, array $options ] )
    public int removeBin ( array $key, array $bins [, array $options ] )
    public int increment ( array $key, string $bin, int $offset [, array $options ] )
    public int append ( array $key, string $bin, string $value [, array $options ] )
    public int prepend ( array $key, string $bin, string $value [, array $options ] )
    public int operate ( array $key, array $operations [, array &$returned ] )

    // batch operation methods
    public int getMany ( array $keys, array &$records [, array $filter [, array $options]] )
    public int existsMany ( array $keys, array &$metadata [, array $options ] )
}
```

### [Runtime Configuration](aerospike_config.md)
### [Lifecycle and Connection Methods](apiref_connection.md)
### [Error Handling and Logging Methods](apiref_error.md)
### [Key-Value Methods](apiref_kv.md)

An overview of the development of the client is at the top level
[README](README.md).
