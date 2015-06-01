
# Overview

The Aerospike <a href="http://www.aerospike.com/docs/architecture/clients.html"
target="_doc">HHVM client</a> enables your PHP application to work with an
<a href="http://www.aerospike.com/docs/architecture/distribution.html"
target="_doc">Aerospike cluster</a> as its
<a href="http://www.aerospike.com/docs/guide/kvs.html" target="_doc">key-value store</a>.

The <a href="http://www.aerospike.com/docs/architecture/data-model.html" target="_doc">Data Model</a>
document gives further details on how data is organized in the cluster.

## Client API
The Aerospike client for HHVM will eventually implement the full API
described in the 
[aerospike/aerospike-client-php](https://github.com/aerospike/aerospike-client-php/blob/master/doc/README.md)
repository. Currently a subset of the API has been implemented by the HNI
extension.

### [Runtime Configuration](aerospike_config.md)
### [Aerospike Class](aerospike.md)
### [Lifecycle and Connection Methods](apiref_connection.md)
### [Error Handling Methods](apiref_error.md)
### [Key-Value Methods](apiref_kv.md)

## Implementation Status
So far the *Runtime Configuration*, *Lifecycle and Connection Methods*, *Error*
*Handling and Logging Methods*, and *Key-Value Methods* have been implemented.

## Persistent Connections

Initializing the C-client to connect to a specified cluster is a costly
operation, so ideally the C-client should be reused for the multiple requests
made against the same PHP process (as is the case for mod_php and fastCGI).

The PHP developer can determine whether the Aerospike class constructor will
use persistent connections or not by way of an optional boolean argument.
After the first time Aerospike::__construct() is called within the process, the
extension will attempt to reuse the persistent connection.

When persistent connections are used the methods _reconnect()_ and _close()_ do
not actually close the connection.  Those methods only apply to instances of
class Aerospike which use non-persistent connections.

## Handling Unsupported Types
The HHVM client does not yet handle types such as float, boolean and object.
In a future release it will behave similar to the way the
[PHP client does](https://github.com/aerospike/aerospike-client-php/blob/master/doc/README.md#handling-unsupported-types).

## Further Reading

- [How does the Aerospike client find a node](https://discuss.aerospike.com/t/how-does-aerospike-client-find-a-node/706)
- [How would hash collisions be handled](https://discuss.aerospike.com/t/what-will-aerospike-do-if-hash-collision-for-a-key/779)
