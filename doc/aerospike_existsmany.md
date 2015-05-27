
# Aerospike::existsMany

Aerospike::existsMany - check if a batch of records exist in the Aerospike database

## Description

```
public int Aerospike::existsMany ( array $keys, array &$metadata [, array $options ] )
```

**Aerospike::existsMany()** will check if a batch of records from a list of given *keys*
exists in the database.
If the key exists its metadata will be returned in the *metadata* variable,
and non-existent records will return as NULL.

## Parameters

**keys** an array of initialized keys, each an array with keys ['ns','set','key'] or ['ns','set','digest'].

**metadata** filled by an array of metadata.

**[options](aerospike.md)** including
- **Aerospike::OPT_READ_TIMEOUT**

## Return Values

Returns an integer status code.  Compare to the Aerospike class status
constants.  When non-zero the **Aerospike::error()** and
**Aerospike::errorno()** methods can be used.

## Examples

```php
<?php

$config = array("hosts"=>array(array("addr"=>"localhost", "port"=>3000)));
$db = new Aerospike($config);
if (!$db->isConnected()) {
   echo "Aerospike failed to connect[{$db->errorno()}]: {$db->error()}\n";
   exit(1);
}

$key1 = $db->initKey("test", "users", 1234);
$key2 = $db->initKey("test", "users", 1235); // this key does not exist
$key2 = $db->initKey("test", "users", 1236);
$keys = array($key1, $key2, $key3);
$status = $db->existsMany($keys, $metadata);
if ($status == Aerospike::OK) {
    var_dump($metadata);
} else {
    echo "[{$db->errorno()}] ".$db->error();
}

?>
```

We expect to see:

```
array(3) {
  [1234]=>
  array(2) {
    ["generation"]=>
    int(4)
    ["ttl"]=>
    int(1337)
  }
  [12345]=>
  NULL
  [1236]=>
  array(2) {
    ["generation"]=>
    int(3)
    ["ttl"]=>
    int(505)
  }
}
```

