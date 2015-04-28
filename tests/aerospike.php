<?php

$SERVER_IP = "localhost";
$SERVER_PORT = 3000;

$conf = array("hosts"=>array(array("addr"=>$SERVER_IP, "port"=>$SERVER_PORT)));
$config_options = array(Aerospike::OPT_WRITE_TIMEOUT=>8000);

$db = new Aerospike($conf, $config_options);
if (!$db->isConnected()) {
    echo "Connection not established!\n";
    echo "Error ". $db->errorno() . " : " . $db->error() . "\n" ;
    exit -1;
} else {
    echo "Connection is established!\n";
    $key =  $db->initKey("test", "test-set", "test-key");
    var_dump($key);
    $rec = array("test-bin-1"=>"test-value-1",
                 "test-bin-2"=>1000,
                 "test-bin-3"=>array("name"=>"John",
                                     "hobby"=>"dance",
                                     4=>1000,
                                     "skills"=>array("c", "cpp", "python", "php"),
                                     "grades"=>array("total"=>8, "major"=>9)),
                 "test-bin-4"=>array("hello", "world", 120,
                                     array("Data Structures", "Networking", "Operating Systems"),
									 array("cities"=>array("Atlanta", "Boston", "Newyork"), "age"=>25)));
	$options = array(Aerospike::OPT_WRITE_TIMEOUT=>5000);
    $status = $db->put($key, $rec, NULL, $options);
    if ($status != Aerospike::OK) {
        echo "Error ". $db->errorno() . " : " . $db->error() . "\n" ;
    }

    $options = array(Aerospike::OPT_READ_TIMEOUT=>8999);
    $status = $db->get($key, $data, $options);
    if ($status != Aerospike::OK) {
        echo "Error ". $db->errorno() . " : " . $db->error() . "\n" ;
    } else {
        var_dump($data);
	}
}
$db->close();

?>

