
# Aerospike::isConnected

Aerospike::isConnected - Tests the connection to the Aerospike DB

## Description

```
public boolean Aerospike::isConnected ( void )
```

**Aerospike::isConnected()** can be used to test the connection to the Aerospike
DB. If a connection error occured the **Aerospike::error()** and
**Aerospike::errorno()** methods can be used to inspect it.

## Parameters

This method has no parameters.

## Return Values

Returns a **true** or **false**.

## See Also

- [Aerospike::isConnected()](aerospike_isconnected.md)

## Examples

```php
<?php

$config = array("hosts"=>array(array("addr"=>"localhost", "port"=>3000)));
$opts = array(Aerospike::OPT_CONNECT_TIMEOUT => 750);
$db = new Aerospike($config, true, $opts);
if (!$db->isConnected()) {
   echo "Aerospike failed to connect[{$db->errorno()}]: {$db->error()}\n";
   exit(1);
}

?>
```

On error we expect to see:

```
Aerospike failed to connect[300]: failed to initialize cluster
```

