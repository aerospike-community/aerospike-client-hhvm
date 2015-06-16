
# Aerospike::scanInfo

Aerospike::scanInfo - gets the status of a background scan triggered by scanApply()

## Description

```
public int Aerospike::scanInfo ( integer $scan_id, array &$info [, array $options ] )
```

**Aerospike::scanInfo()** will return *information* on a background scan identified
by *scan_id* which was triggered using **Aerospike::scanApply()**.

## Parameters

**scan_id** the scan id

**info** the status of the background scan returned as an array conforming to the following:
```
Associative Array:
  progress_pct => progress percentage for the scan
  records_scanned => number of records scanned
  status => one of Aerospike::SCAN_STATUS_*
```

**[options](aerospike.md)** including
- **Aerospike::OPT_READ_TIMEOUT**

## Return Values

Returns an integer status code.  Compare to the Aerospike class status
constants.  When non-zero the **Aerospike::error()** and
**Aerospike::errorno()** methods can be used.

## See Also

- [Aerospike::scanApply()](aerospike_scanapply.md)

## Examples

```php
<?php

$config = array("hosts"=>array(array("addr"=>"localhost", "port"=>3000)));
$db = new Aerospike($config);
if (!$db->isConnected()) {
   echo "Aerospike failed to connect[{$db->errorno()}]: {$db->error()}\n";
   exit(1);
}

$poll = true;
while($poll) {
    $status = $db->scanInfo(1, $info);
    if ($status == Aerospike::OK) {
        var_dump($info);
        if ($info["status"] == Aerospike::SCAN_STATUS_COMPLETED) {
            echo "Background scan is complete!";
            $poll = false;
        }
    } else {
        echo "An error occured while retrieving info of scan [{$db->errorno()}] {$db->error()}\n";
        $poll = false;
    }
}

?>
```

We expect to see:

```
array(3) {
  ["progress_pct"]=>
  int(70)
  ["records_scanned"]=>
  int(1000)
  ["status"]=>
  int(1)
}
```

