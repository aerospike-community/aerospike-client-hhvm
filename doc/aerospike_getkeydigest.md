
# Aerospike::getKeyDigest

Aerospike::getKeyDigest - helper method for building the key array

## Description

```
public string Aerospike::getKeyDigest (string $ns, string $set, string|int $pk)
```

**Aerospike::getKeyDigest()** will return the RIPEMD-160 digest corresponding to
the hash of the key tuple. The digest is used to uniquely identify the record in
the cluster.

## Parameters

**ns** the namespace

**set** the name of the set within the namespace

**pk** the primary key that identifies the record in the application

## Return Value

The RIPEMD-160 digest, stored as a binary string.

## Examples

### Initializing a key
```php
<?php

$config = array("hosts"=>array(array("addr"=>"localhost", "port"=>3000)));
$db = new Aerospike($config);
if (!$db->isConnected()) {
   echo "Aerospike failed to connect[{$db->errorno()}]: {$db->error()}\n";
   exit(1);
}

$digest = $db->getKeyDigest("test", "users", 1);
$key = $db->initKey("test", "users", $digest, true);
var_dump($digest, $key);

?>
```

We expect to see:

```
string(20) "9!?@%??;???Wp?'??Ag"
array(3) {
  ["ns"]=>
  string(4) "test"
  ["set"]=>
  string(5) "users"
  ["digest"]=>
  string(20) "9!?@%??;???Wp?'??Ag"
}
```

