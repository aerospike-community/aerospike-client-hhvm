
# Lifecycle and Connection Methods

### [Aerospike::__construct](aerospike_construct.md)
```
public int Aerospike::__construct ( array $config [, boolean $persistent_connection = true [, array $options]] )
```

### [Aerospike::__destruct](aerospike_destruct.md)
```
public Aerospike::__destruct ( void )
```

### [Aerospike::isConnected](aerospike_isconnected.md)
```
public boolean Aerospike::isConnected ( void )
```

### [Aerospike::close](aerospike_close.md)
```
public Aerospike::close ( void )
```

### [Aerospike::reconnect](aerospike_reconnect.md)
```
public Aerospike::reconnect ( void )
```

## Example

```php
<?php

$config = array("hosts"=>array(array("addr"=>"192.168.1.10", "port"=>3000)));
$db = new Aerospike($config, false);
if (!$db->isConnected()) {
   echo "Aerospike failed to connect[{$db->errorno()}]: {$db->error()}\n";
   exit(1);
}

$db->close();
$db->reconnect();
$db->__destruct();

?>
```
