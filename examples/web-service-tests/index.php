<?php
require '/usr/local/Slim/Slim/Slim.php';
\Slim\Slim::registerAutoloader(); 

$app = new \Slim\Slim();

$config = array("hosts"=>array(array("addr"=>"10.73.80.157", "port"=>"3000")));
$db = new Aerospike($config);


$app->get('/persistent', function () {
    $config = array("hosts"=>array(array("addr"=>"10.73.80.157", "port"=>"3000")));
    $db = new Aerospike($config);
    if (!$db->isConnected()) {
        echo "Aerospike failed to connect[{$db->errorno()}]: {$db->error()}\n";
        echo "Not connected";
        exit(1);
    } else {
        echo "Connected";
        echo "<h1>Welcome!</h1>";
    }
});

$app->get('/nonpersistent', function () {
    $config = array("hosts"=>array(array("addr"=>"10.73.80.157", "port"=>"3000")));
    $db = new Aerospike($config, false);
    if (!$db->isConnected()) {
        echo "Aerospike failed to connect[{$db->errorno()}]: {$db->error()}\n";
        echo "Not connected";
        exit(1);
    } else {
        echo "Connected";
        echo "<h1>Welcome!</h1>";
    }
});

$app->get('/', function () use($db) {
    if (!$db->isConnected()) {
        echo "Aerospike failed to connect[{$db->errorno()}]: {$db->error()}\n";
        echo "Not connected";
        exit(1);
    } else {
        echo "Connected";
        echo "<h1>Welcome!</h1>";
    }
});

$app->get('/get', function() use($db) {
    if($db->isConnected()) {
        $key = $db->initKey("test", "demo", "key1");
        $status = $db->get($key, $record);
        var_dump($record);
        if($status != Aerospike::OK) {
            echo "Get failed";
        }
    $db->close();
    }
});

$app->get('/multiget', function() use($db) {
    echo "In multiget";
    if($db->isConnected()) {
        for($i=1 ;$i<1000; $i++) {
            $key = $db->initKey("test", "demo", "key".$i);
            $status = $db->get($key, $record);
            var_dump($record);
            if($status != Aerospike::OK) {
                echo "\nMultiget failed at ".$i."th record";
            }
        }
    $db->close();
    }
});

$app->get('/getmany', function() use($db) {
    echo "In getMany";
    if($db->isConnected()) {
	$keys=[];
        for($i=1 ;$i<1000; $i++) {
            $keys[] = $db->initKey("test", "demo", "key".$i);
        }
        $status = $db->getMany($keys, $records);
        var_dump($records);
        if($status != Aerospike::OK) {
            echo "\ngetMany failed!";
        }
	$db->close();
    }
});

$app->get('/existsmany', function() use($db) {
    echo "In existsMany";
    if($db->isConnected()) {
	$keys=[];
        for($i=1 ;$i<1000; $i++) {
            $keys[] = $db->initKey("test", "demo", "key".$i);
        }
        $status = $db->existsMany($keys, $meta);
        var_dump($meta);
        if($status != Aerospike::OK) {
            echo "\nexistsMany failed!";
        }
	$db->close();
    }
});

$app->put('/increment', function() use($db) {
    $key = $db->initKey("test", "demo", "key1");
    $res = $db->increment($key, 'age', 20);
    if ($res != Aerospike::OK) {
        echo "Error is: \n".$db->errorno();;
    }
    $db->close();
});

$app->post('/put', function() use($db) {
    if($db->isConnected()) {
        $key = $db->initKey("test", "demo", "key1");
        $record = array("name" => "John", "age" => 32);
        $status = $db->put($key, $record);
        if($status != Aerospike::OK) {
            echo "Single put failed";
        }
        $db->close();
    }
});

$app->post('/multiput', function() use($db) {
    if($db->isConnected()) {
        for($i=1; $i<1000; $i++) {
            $key = $db->initKey("test", "demo", "key".$i);
            $record = array("name" => "name".$i, "age" => $i);
            $status = $db->put($key, $record);
            if($status != Aerospike::OK) {
                echo "\nMultiput failed at ".$i."th record";
                break;
            }
        }
    $db->close();
    }
});

$app->put('/multiappend', function() use($db) {
    if ($db->isConnected()) {
        for ($i=1; $i<1000; $i++) {
            $key = $db->initKey("test", "demo", "key".$i);
            $status = $db->append($key, 'name', ' World',
                array(Aerospike::OPT_WRITE_TIMEOUT=>1000));
            if($status != Aerospike::OK) {
                echo "\nAppend failed at ".$i."th record";
            }
        }
    $db->close();
    }
});


$app->put('/multiprepend', function() use($db) {
    if ($db->isConnected()) {
        for ($i=1; $i<1000; $i++) {
        $key = $db->initKey("test", "demo", "key".$i);
        $status = $db->prepend($key, 'name', 'Greet',
            array(Aerospike::OPT_WRITE_TIMEOUT=>1000));
        if($status != Aerospike::OK) {
            echo "\nPrepend failed at ".$i."th record";
        }
        }
    $db->close();
    }
});

$app->put('/multitouch', function() use($db) {
    if ($db->isConnected()) {
        for($i=1; $i<1000; $i++) {
            $key = $db->initKey("test", "demo", "key".$i);
            $status = $db->touch($key, 1000, array(Aerospike::OPT_WRITE_TIMEOUT=>200));
            if($status != Aerospike::OK) {
                echo "Multitouch failed at ".$i."th record";
                break;
            }
        }
        $db->close();
    }
});

$app->put('/removebin', function() use($db) {
    if ($db->isConnected()) {
        for($i=1; $i<1000; $i++) {
            $key = $db->initKey("test", "demo", "key".$i);
            $status = $db->removeBin($key, array("age"));
            if($status != Aerospike::OK) {
                echo "RemoveBin failed at ".$i."th record";
                break;
            }
        }
        $db->close();
    }
});

$app->put('/remove', function() use($db) {
    if ($db->isConnected()) {
        for($i=1; $i<1000; $i++) {
            $key = $db->initKey("test", "demo", "key".$i);
            $status = $db->remove($key);
            if($status != Aerospike::OK) {
                echo "Remove failed at ".$i."th record";
                break;
            }
        }
        $db->close();
    }
});

$app->put('/closeconnection', function() use($db) {
    $db->close();
    echo "Connection closed";
});

$app->get('/reconnect', function() use($db) {
    $db->reconnect();
    echo "Connection re-established";
});

$app->run();
?>
