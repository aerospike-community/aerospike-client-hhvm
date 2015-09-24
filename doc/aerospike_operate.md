
# Aerospike::operate

Aerospike::operate - multiple operations on a single record

## Description

```
public int Aerospike::operate ( array $key, array $operations [, array &$returned [, array $options]] )
```

**Aerospike::operate()** allows for multiple per-bin operations on a *record*
with a given *key*, with write operations happening before read ones.
Non-existent bins being read will have a NULL value.

Currently only a call to operate() can include only one write operation per-bin.
For example, you cannot both append and prepend to the same bin, in the same
call.

Like other bin operations, operate() only works on existing records
(i.e. ones that were previously created with a put()).

## Parameters

**key** the key identifying the record. An array with keys ['ns','set','key'] or ['ns','set','digest'].

**operations** an array of one or more per-bin operations conforming
to the following structure:
```
Write Operation:
  op => Aerospike::OPERATOR_WRITE
  bin => bin name (cannot be longer than 14 characters)
  val => the value to store in the bin

Increment Operation:
  op => Aerospike::OPERATOR_INCR
  bin => bin name
  val => the integer by which to increment the value in the bin

Prepend Operation:
  op => Aerospike::OPERATOR_PREPEND
  bin => bin name
  val => the string to prepend the string value in the bin

Append Operation:
  op => Aerospike::OPERATOR_APPEND
  bin => bin name
  val => the string to append the string value in the bin

Read Operation:
  op => Aerospike::OPERATOR_READ
  bin => name of the bin we want to read after any write operations

Touch Operation: reset the time-to-live of the record and increment its generation
  op => Aerospike::OPERATOR_TOUCH
  ttl => a positive integer value to set as time-to-live for the record 
```
*examples:*
```
array(
  array("op" => Aerospike::OPERATOR_APPEND, "bin" => "name", "val" => " Ph.D."),
  array("op" => Aerospike::OPERATOR_INCR, "bin" => "age", "val" => 1),
  array("op" => Aerospike::OPERATOR_READ, "bin" => "age"),
  array("op" => Aerospike::OPERATOR_TOUCH, "ttl" => 20)
)
```

**returned** an array of bins retrieved by read operations

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
$operations = array(
  array("op" => Aerospike::OPERATOR_APPEND, "bin" => "name", "val" => " Ph.D."),
  array("op" => Aerospike::OPERATOR_INCR, "bin" => "age", "val" => 1),
  array("op" => Aerospike::OPERATOR_READ, "bin" => "age"),
  array("op" => Aerospike::OPERATOR_TOUCH, "ttl" => 20)
);
$status = $db->operate($key, $operations, $returned);
if ($status == Aerospike::OK) {
    var_dump($returned);
} else {
    echo "[{$db->errorno()}] ".$db->error();
}

?>
```

We expect to see:

```
array(1) {
  ["age"]=>
  int(34)
}
```

