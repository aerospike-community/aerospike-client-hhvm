
# Aerospike::errorno

Aerospike::errorno - display an error code associated with the last operation

## Description

```
public int Aerospike::errorno ( void )
```

**Aerospike::errorno()** will return an error code associated with the last
operation. If the operation was successful the return value should be 0
(**Aerospike::OK**).

## Return Values

Returns an error code, which may be 0 (**Aerospike::OK**) on success.

## Examples

```php
<?php

$config = array("hosts"=>array(array("addr"=>"localhost", "port"=>3000)));
$db = new Aerospike($config);
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

