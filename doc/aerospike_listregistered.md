
# Aerospike::listRegistered

Aerospike::listRegistered - lists the UDF modules registered with the server

## Description

```
public int Aerospike::listRegistered ( array &$modules [, int $language [, array $options ]] )
```

**Aerospike::listRegistered()** populates *modules* with the list of UDF modules
registered with the server.

## Parameters

**modules** filled by an array of module info.

**language** one of *Aerospike::UDF_TYPE_\**.  Optionally filters a subset of
modules matching the given type.

**[options](aerospike.md)** including
- **Aerospike::OPT_READ_TIMEOUT**

## Return Values

Returns an integer status code.  Compare to the Aerospike class status
constants.  When non-zero the **Aerospike::error()** and
**Aerospike::errorno()** methods can be used.

The modules array has multiples of the following structure:
```
Array of:
  name => module name
  type => Aerospike::UDF_TYPE_*
```

## Examples

```php
<?php

$config = array("hosts"=>array(array("addr"=>"localhost", "port"=>3000)));
$db = new Aerospike($config);
if (!$db->isConnected()) {
   echo "Aerospike failed to connect[{$db->errorno()}]: {$db->error()}\n";
   exit(1);
}

$status = $db->listRegistered($modules);
if ($status == Aerospike::OK) {
    var_dump($modules);
} else {
    echo "[{$db->errorno()}] ".$db->error();
}

?>
```

We expect to see:

```
array(3) {
  [0]=>
  array(2) {
    ["name"]=>
    string(13) "my_record_udf"
    ["type"]=>
    int(1)
  }
  [1]=>
  array(2) {
    ["name"]=>
    string(13) "my_stream_udf"
    ["type"]=>
    int(1)
  }
  [2]=>
  array(2) {
    ["name"]=>
    string(5) "utils"
    ["type"]=>
    int(1)
  }
}
```


