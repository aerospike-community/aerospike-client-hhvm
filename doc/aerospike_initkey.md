
# Aerospike::initKey

Aerospike::initKey - helper method for building the key array

## Description

```
public array Aerospike::initKey ( string $ns, string $set, int|string $pk [, boolean $is_digest = false ] )
```

**Aerospike::initKey()** will return an array that can be passed as the
*$key* arguement in [key-value methods](apiref_kv.md).

## Parameters

**ns** the namespace

**set** the name of the set within the namespace

**pk** the primary key or digest value that identifies the record

**is_digest** true if the *pk* argument is a digest, false if it is a key

## Return Values

Returns an array with the following structure:
```
Array:
  ns => string namespace
  set => string set name
  key => the record's primary key value
```
or
```
Array:
  ns => string namespace
  set => string set name
  digest => RIPEMD-160 hash of the key (a 20 byte string)
```
or *NULL* on failure.

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

$key = $db->initKey("test", "users", 1234);
var_dump($key);

?>
```

We expect to see:

```
array(3) {
  ["ns"]=>
  string(4) "test"
  ["set"]=>
  string(5) "users"
  ["key"]=>
  int(1234)
}
```

### Initializing a digest
```php
<?php

$config = array("hosts"=>array(array("addr"=>"localhost", "port"=>3000)));
$db = new Aerospike($config);
if (!$db->isConnected()) {
   echo "Aerospike failed to connect[{$db->errorno()}]: {$db->error()}\n";
   exit(1);
}

$base64_encoded_digest = '7EV9CpdMSNVoWn76A9E33Iu95+M=';
$digest = base64_decode($base64_encoded_digest);
$key = $db->initKey("test", "users", $digest, true);
var_dump($key);

?>
```

We expect to see:

```
array(3) {
  ["ns"]=>
  string(4) "test"
  ["set"]=>
  string(5) "users"
  ["digest"]=>
  string(20) "?E}
?LH?hZ~??7܋???"
}
```

