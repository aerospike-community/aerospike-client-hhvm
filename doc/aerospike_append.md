
# Aerospike::append

Aerospike::append - appends a string to the string value in a bin

## Description

```
public int Aerospike::append ( array $key, string $bin, string $value [, array $options ] )
```

**Aerospike::append()** will append a string to the string value in *bin*.
Like other bin operations, append() only works on existing records
(i.e. ones that were previously created with a put()).


## Parameters

**key** the key under which the record can be found. An array with keys ['ns','set','key'] or ['ns','set','digest'].

**bin** the name of the bin in which we have a numeric value.

**value** the string to append to the string value in the bin.

**[options](aerospike.md)** including
- **Aerospike::OPT_WRITE_TIMEOUT**
- **[Aerospike::OPT_POLICY_RETRY](http://www.aerospike.com/apidocs/c/db/d65/group__client__policies.html#gaa9730980a8b0eda8ab936a48009a6718)**
- **[Aerospike::OPT_POLICY_KEY](http://www.aerospike.com/apidocs/c/db/d65/group__client__policies.html#gaa9c8a79b2ab9d3812876c3ec5d1d50ec)**
- **[Aerospike::OPT_POLICY_GEN](http://www.aerospike.com/apidocs/c/db/d65/group__client__policies.html#ga38c1a40903e463e5d0af0141e8c64061)**
- **[Aerospike::OPT_POLICY_REPLICA](http://www.aerospike.com/apidocs/c/db/d65/group__client__policies.html#gabce1fb468ee9cbfe54b7ab834cec79ab)**
- **[Aerospike::OPT_POLICY_CONSISTENCY](http://www.aerospike.com/apidocs/c/db/d65/group__client__policies.html#ga34dbe8d01c941be845145af643f9b5ab)**
- **[Aerospike::OPT_POLICY_COMMIT_LEVEL](http://www.aerospike.com/apidocs/c/db/d65/group__client__policies.html#ga17faf52aeb845998e14ba0f3745e8f23)**

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

$key = $db->initKey("test", "users", 1234);
$status = $db->append($key, 'name', ' Ph.D.');
if ($status == Aerospike::OK) {
    echo "Added the Ph.D. suffix to the user.\n";
} else {
    echo "[{$db->errorno()}] ".$db->error();
}

?>
```

We expect to see:

```
Added the Ph.D. suffix to the user.
```

