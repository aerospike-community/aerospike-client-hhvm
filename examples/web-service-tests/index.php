<?php
require '/usr/local/Slim/Slim.php';
\Slim\Slim::registerAutoloader(); 

$app = new \Slim\Slim();

$config = array("hosts"=>array(array("addr"=>"127.0.0.1", "port"=>"3000")));
$db = new Aerospike($config);


$app->get('/persistent', function () {
    $config = array("hosts"=>array(array("addr"=>"127.0.0.1", "port"=>"3000")));
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
    $config = array("hosts"=>array(array("addr"=>"127.0.0.1", "port"=>"3000")));
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

$app->get('/scan', function() use($db) {
    if ($db->isConnected()) {
        for($i=1; $i<1000; $i++) {
            $key = $db->initKey("test", "demo", "key".$i);
            $record = array("name" => "name".$i, "age" => $i);
            $status = $db->put($key, $record);
            if($status != Aerospike::OK) {
                echo "\nMultiput failed at ".$i."th record";
                break;
            }
        }
        $status = $db->scan("test", "demo", function ($record) {
            var_dump($record);
        });
        if($status != Aerospike::OK) {
            echo "Scan Failed";
            break;
        }
        $db->close();
    }
});

$app->get('/scanApply', function() use($db) {
    if ($db->isConnected()) {
        for($i=1; $i<1000; $i++) {
            $key = $db->initKey("test", "demo", "key".$i);
            $record = array("name" => "name".$i, "age" => $i);
            $status = $db->put($key, $record);
            if($status != Aerospike::OK) {
                echo "\nMultiput failed at ".$i."th record";
                break;
            }
        }

        //scan_module.lua must be registered with function test_bin_change_name
        $status = $db->scanApply("test", "demo", "scan_module", "test_bin_change_name", array("Aerospike", 50), $scan_id);
        if($status != Aerospike::OK) {
            echo "Scan Apply Failed";
            break;
        }

        $status = $db->scanInfo($scan_id, $scan_info);
        if($status != Aerospike::OK) {
            echo "Scan Info Failed";
            break;
        }
        var_dump($scan_info);
        $db->close();
    }
});

$app->get('/query', function() use($db) {
    if ($db->isConnected()) {
        $key = $db->initKey("test", "demo", "key12");
        $db->put($key,array("first_name"=>"john", "age"=>22));
        $key = $db->initKey("test", "demo", "key15");
        $db->put($key,array("first_name"=>"john", "age"=>25));

        $where = $db->predicateEquals("age", 25);

        $status = $db->query("test", "demo", $where, function ($record) {
            var_dump($record);
            return true;
        });
        if($status != Aerospike::OK) {
            var_dump("Query Failed");
            var_dump($db->error());
            var_dump($db->errorno());
        } else {
            var_dump("Query successful");
            var_dump($status);
        }
        $db->close();
    }
});

$app->get('/aggregate', function() use($db) {
    if ($db->isConnected()) {
        $key = $db->initKey("test", "demo", "key12");
        $db->put($key,array("first_name"=>"john", "age"=>22));
        $key = $db->initKey("test", "demo", "key15");
        $db->put($key,array("first_name"=>"john", "age"=>25));

        $where = $db->predicateBetween("age", 20, 29);
        $status = $db->aggregate("test", "demo", $where, "test_stream", "group_count", array("age"), $returned);
        if($status != Aerospike::OK) {
            var_dump("Aggregate Failed");
            var_dump($db->error());
            var_dump($db->errorno());
        } else {
            var_dump("Aggregate successful");
            var_dump($returned);
        }
        $db->close();
    }
});

$app->get('/register', function() use($db) {
    if ($db->isConnected()) {
        ///*
        $status = $db->register("/home/vishal/aerospike-client-hhvm/src/aerospike/tests/lua/test_record_udf.lua", "test_record_udf.lua");
        if($status != Aerospike::OK) {
            var_dump("register failed");
            var_dump($db->error());
            var_dump($db->errorno());
        } else {
            var_dump("UDF register successful");
            var_dump($status);
        }
        // */

        /*
        $status = $db->listRegistered($modules);
        if($status != Aerospike::OK) {
            var_dump("listRegistered failed");
            var_dump($db->error());
            var_dump($db->errorno());
        } else {
            var_dump("UDF listregister successful");
            var_dump($modules);
        }
         */

        /*
        for ($i = 0; $i < count($modules); $i++)
        {
            $status = $db->getRegistered($modules[$i]["name"], $module_code);
            //var_dump($module_code);
        }
         */

        /*
        $key = $db->initKey("test", "demo", "integer_key");
        $record = array("name" => "name100", "age" => 100);
        $status = $db->put($key, $record);
        $status = $db->apply($key, "test_record_udf", "bin_udf_operation_integer", array("age", 2, 20), $returned);
        if ($status != Aerospike::OK) {
            var_dump("apply failed");
            var_dump($db->error());
            var_dump($db->errorno());
        } else {
            var_dump("UDF apply successful");
            var_dump($returned);
        }
        $db->remove($key);
         */

        /*
        $status = $db->deregister("test_record_udf.lua");
        if($status != Aerospike::OK) {
            var_dump("UDF deregister failed");
        } else {
            var_dump("UDF deregister successful");
            var_dump($status);
        }
         */

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
