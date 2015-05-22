
# Runtime Configuration

The following configuration options in php.ini

| Name  | Default  |
|:------|:---------:|
| aerospike.connect_timeout | 1000 |
| aerospike.read_timeout | 1000 |
| aerospike.write_timeout | 1000 |
| aerospike.key_policy | digest |
| aerospike.serializer | php |
| aerospike.udf.lua_system_path | /opt/aerospike/client-php/sys-lua |
| aerospike.udf.lua_user_path | /opt/aerospike/client-php/usr-lua |
| aerospike.shm.use | false |
| aerospike.shm.max_nodes | 16 |
| aerospike.shm.max_namespaces | 8 |
| aerospike.shm.takeover_threshold_sec | 30 |

Here is a description of the configuration directives:

**aerospike.connect_timeout integer**
    The connection timeout in milliseconds

**aerospike.read_timeout integer**
    The read timeout in milliseconds

**aerospike.write_timeout integer**
    The write timeout in milliseconds

**aerospike.key_policy string**
    Whether to send and store the record's (ns,set,key) data along with its (unique identifier) digest. One of { digest, send }

**aerospike.serializer string**
    The unsupported type handler. One of { php, user, none }

**aerospike.udf.lua_system_path string**
    Path to the system support files for Lua UDFs

**aerospike.udf.lua_user_path string**
    Path to the user-defined Lua function modules

**aerospike.shm.use boolean**
    Indicates if shared memory should be used for cluster tending. Recommended for multi-process cases such as FPM. One of { true, false }

**aerospike.shm.max_nodes integer**
    Shared memory maximum number of server nodes allowed. Leave a cushion so new nodes can be added without needing a client restart.

**aerospike.shm.max_namespaces integer**
    Shared memory maximum number of namespaces allowed. Leave a cushion for new namespaces.

**aerospike.shm.takeover_threshold_sec integer**
    Take over shared memory cluster tending if the cluster hasn't been tended by this threshold in seconds.

## See Also

### [Aerospike Class](aerospike.md)
