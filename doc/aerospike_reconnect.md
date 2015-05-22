
# Aerospike::reconnect

Aerospike::reconnect - Reconnect to the Aerospike DB

## Description

```
public Aerospike::reconnect ( void )
```

**Aerospike::reconnect()** will reconnect to the Aerospike DB cluster. The
**Aerospike::isConnected()** method can be used to test whether the connection
succeded.  If a connection error occured the **Aerospike::error()** and
**Aerospike::errorno()** methods can be used to inspect it.

## Parameters

This method has no parameters.

## See Also

- [Aerospike::isConnected()](aerospike_isconnected.md)

## Examples

```php
<?php

$config = array("hosts"=>array(array("addr"=>"localhost", "port"=>3000)));
$opts = array(Aerospike::OPT_CONNECT_TIMEOUT => 1250, Aerospike::OPT_WRITE_TIMEOUT => 1500);
$db = new Aerospike($config, true, $opts);

$db->close();
$db->reconnect();
if (!$db->isConnected()) {
   echo "Aerospike failed to connect[{$db->errorno()}]: {$db->error()}\n";
   exit(1);
}
echo "We have reconnected to the Aerospike DB\n";

?>
```

On success we expect to see:
```
We have reconnected to the Aerospike DB
```

On error we expect to see:

```
Aerospike failed to connect[300]: failed to initialize cluster
```

