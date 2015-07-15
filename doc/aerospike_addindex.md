
# Aerospike::addIndex

Aerospike::addIndex - creates a secondary index on a bin of a specified set

## Description

```
public int Aerospike::addIndex ( string $ns, string $set, string $bin, string $name, int $index_type, int $data_type [, array $options ] )
```

**Aerospike::addIndex()** will create a secondary index of a given *index_type* on
a namespace *ns*, *set* and *bin* with a specified *name*.

## Parameters

**ns** the namespace

**set** the set

**bin** the bin on which the secondary index is to be created

**name** the name of the index

**index_type** one of *Aerospike::INDEX_TYPE_\** .

**data_type** one of *Aerospike::INDEX_NUMERIC* and *Aerospike::INDEX_STRING*

**options** including
- **Aerospike::OPT_WRITE_TIMEOUT**

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

$status = $db->addIndex("test", "user", "email", "user_email_idx", Aerospike::INDEX_TYPE_DEFAULT, Aerospike::INDEX_STRING);
if ($status == Aerospike::OK) {
    echo "Index user_email_idx created on test.user.email\n";
else if ($status == Aerospike::ERR_INDEX_FOUND) {
    echo "This index has already been created.\n";
} else {
    echo "[{$db->errorno()}] ".$db->error();
}

$db->addIndex("test", "user", "movies", "user_movie_titles_idx", Aerospike::INDEX_TYPE_MAPKEYS, Aerospike::INDEX_STRING);
$db->addIndex("test", "user", "movies", "user_movie_views_idx", Aerospike::INDEX_TYPE_MAPVALUES, Aerospike::INDEX_NUMERIC);
$db->addIndex("test", "user", "aliases", "user_aliases_idx", Aerospike::INDEX_TYPE_LIST, Aerospike::INDEX_STRING);

?>
```

