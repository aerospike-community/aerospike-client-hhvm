<?php
$SERVER_IP = "localhost";
$SERVER_PORT = 3000;
$conf = array("hosts"=>array(array("addr"=>$SERVER_IP, "port"=>$SERVER_PORT)));
$db = new Aerospike($conf);

if (!$db->isConnected()) {
    echo "Connection not established!";
    echo "Error ". $db->errorno() . " : " . $db->error() . "\n" ;
    exit -1;
} else {
    $key =  $db->initKey("test", "test-set", "test-key");
    $rec = array("test-bin-1"=>"test-value-1", "test-bin-2"=>"test-value-2", "test-bin-3"=>100);
    $status = $db->put($key, $rec);
    if ($status != Aerospike::OK) {
        echo "Error ". $db->errorno() . " : " . $db->error() . "\n" ;
    }

    $data=array();
    $status = $db->get($key, $data);
    if ($status != Aerospike::OK) {
        echo "Error ". $db->errorno() . " : " . $db->error() . "\n" ;
    } else {
        var_dump($data);
    }
}
$db->close();

?>

