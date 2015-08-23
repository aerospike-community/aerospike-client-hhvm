
# Admin Methods

### [Aerospike::addIndex](aerospike_addindex.md)
```
public int Aerospike::addIndex ( string $ns, string $set, string $bin, string $name, int $index_type, int $data_type [, array $options ] )
```

### [Aerospike::dropIndex](aerospike_dropindex.md)
```
public int Aerospike::dropIndex ( string $ns, string $name )
```

## Example

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

?>
```
