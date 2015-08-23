<?php
require '/usr/local/Slim/Slim.php';
\Slim\Slim::registerAutoloader(); 

$app = new \Slim\Slim();

$LUA_DIR = "/home/user";
$config = array("hosts"=>array(array("addr"=>"127.0.0.1", "port"=>"3000")));
$db = new Aerospike($config);

$app->get('/', function () use($db) {
    if (!$db->isConnected()) {
        echo "Aerospike failed to connect[{$db->errorno()}]: {$db->error()}\n";
        exit(1);
    } else {
        echo "Connected to the Aerospike Database";
        echo "<h1>Welcome!</h1>";
    }
});


$app->get('/persistent', function () {
    $config = array("hosts"=>array(array("addr"=>"127.0.0.1", "port"=>"3000")));
    $db = new Aerospike($config);
    if (!$db->isConnected()) {
        echo "Aerospike failed to connect[{$db->errorno()}]: {$db->error()}\n";
        exit(1);
    } else {
        echo "Connected to the Aerospike Database";
        echo "<h1>Welcome!</h1>";
    }
});

$app->get('/nonpersistent', function () {
    $config = array("hosts"=>array(array("addr"=>"127.0.0.1", "port"=>"3000")));
    $db = new Aerospike($config, false);
    if (!$db->isConnected()) {
        echo "Aerospike failed to connect[{$db->errorno()}]: {$db->error()}\n";
        exit(1);
    } else {
        echo "Connected to the Aerospike Database";
        echo "<h1>Welcome!</h1>";
    }
});

$app->post('/put', function() use($db) {
    if ($db->isConnected()) {
        $key = $db->initKey("test", "demo", "key1");
        $record = array("name" => "John", "age" => 32);
        $status = $db->put($key, $record);
        if ($status != Aerospike::OK) {
            echo "Single put failed";
        } else {
            echo "Record has been successfully written to the Database\n";
        }
        $db->close();
    } else {
        echo "Aerospike DB connection is not established\n";
    }
});

$app->post('/multiput', function() use($db) {
    if ($db->isConnected()) {
        for($i = 0; $i < 100; $i++) {
            $key = $db->initKey("test", "demo", "key".$i);
            $record = array("name" => "name".$i, "age" => $i);
            $status = $db->put($key, $record);
            if ($status != Aerospike::OK) {
                echo "\nMultiput failed at ".$i."th record";
                break;
            }
        }
        $db->close();
    } else {
        echo "Aerospike DB connection is not established";
    }
});

$app->get('/get', function() use($db) {
    if ($db->isConnected()) {
        $key = $db->initKey("test", "demo", "key1");
        $status = $db->get($key, $record);
        var_dump($record);
        if ($status != Aerospike::OK) {
            echo "Get operation failed";
        }
        $db->close();
    } else {
        echo "Aerospike DB connection is not established";
    }
});

$app->get('/multiget', function() use($db) {
    if ($db->isConnected()) {
        for ($i = 0 ;$i < 100; $i++) {
            $key = $db->initKey("test", "demo", "key".$i);
            $status = $db->get($key, $record);
            var_dump($record);
            if ($status != Aerospike::OK) {
                echo "\nMultiget failed at ".$i."th record";
            }
        }
        $db->close();
    } else {
        echo "Aerospike DB connection is not established";
    }
});

$app->put('/append', function() use($db) {
    if ($db->isConnected()) {
        $key = $db->initKey("test", "demo", "key_append");
        $record = array("name" => "John", "age" => 32);
        $status = $db->put($key, $record);
        $status = $db->append($key, 'name', ' Watson', array(Aerospike::OPT_WRITE_TIMEOUT=>1000));
        if ($status != Aerospike::OK) {
            echo "\nAppend operation failed";
        } else {
            echo "\nValue appended successfully";
            $db->get($key, $record);
            var_dump($record);
        }
        $db->remove($key);
        $db->close();
    } else {
        echo "Aerospike DB connection is not established";
    }
});

$app->put('/multiappend', function() use($db) {
    if ($db->isConnected()) {
        for ($i = 0; $i < 100; $i++) {
            $key = $db->initKey("test", "demo", "key".$i);
            $status = $db->append($key, 'name', ' World',
                array(Aerospike::OPT_WRITE_TIMEOUT=>1000));
            if ($status != Aerospike::OK) {
                echo "\nAppend failed at ".$i."th record";
            }
        }
        $db->close();
    } else {
        echo "Aerospike DB connection is not established";
    }
});

$app->put('/prepend', function() use($db) {
    if ($db->isConnected()) {
        $key = $db->initKey("test", "demo", "key_prepend");
        $record = array("name" => "John", "age" => 32);
        $status = $db->put($key, $record);
        $status = $db->prepend($key, 'name', 'Dr. ', array(Aerospike::OPT_WRITE_TIMEOUT=>1000));
        if ($status != Aerospike::OK) {
            echo "\nPrepend operation failed";
        } else {
            echo "\nValue prepended successfully";
            $db->get($key, $record);
            var_dump($record);
        }
        $db->remove($key);
        $db->close();
    } else {
        echo "Aerospike DB connection is not established";
    }
});
$app->put('/multiprepend', function() use($db) {
    if ($db->isConnected()) {
        for ($i = 0; $i < 100; $i++) {
            $key = $db->initKey("test", "demo", "key".$i);
            $status = $db->prepend($key, 'name', 'Greet', array(Aerospike::OPT_WRITE_TIMEOUT=>1000));
            if ($status != Aerospike::OK) {
                echo "\nPrepend failed at ".$i."th record";
            }
        }
        $db->close();
    } else {
        echo "Aerospike DB connection is not established";
    }
});

$app->put('/increment', function() use($db) {
    if ($db->isConnected()) {
        $key = $db->initKey("test", "demo", "key1");
        $res = $db->increment($key, 'age', 20);
        if ($res != Aerospike::OK) {
            echo "Error is: \n".$db->errorno();;
        } else {
            echo "\nValue Incremented successfully";
            $db->get($key, $record);
            var_dump($record);
        }
        $db->close();
    } else {
        echo "Aerospike DB connection is not established";
    }
});

$app->put('/multiincrement', function() use($db) {
    if ($db->isConnected()) {
        for ($i = 0 ;$i < 100; $i++) {
            $key = $db->initKey("test", "demo", "key".$i);
            $status = $db->increment($key, 'age', 20);
            if ($status != Aerospike::OK) {
                echo "\nMultiincrement failed at ".$i."th record";
            }
        }
        $db->close();
    } else {
        echo "Aerospike DB connection is not established";
    }
});

$app->put('/touch', function() use($db) {
    if ($db->isConnected()) {
        $key = $db->initKey("test", "demo", "key_touch");
        $record = array("name" => "John", "age" => 32);
        $status = $db->put($key, $record);
        $status = $db->touch($key, 1000, array(Aerospike::OPT_WRITE_TIMEOUT=>200));
        if ($status != Aerospike::OK) {
            echo "Touch operation failed";
            break;
        } else {
            echo "\nTouch operation successful";
        }
        $db->remove($key);
        $db->close();
    } else {
        echo "Aerospike DB connection is not established";
    }
});

$app->put('/multitouch', function() use($db) {
    if ($db->isConnected()) {
        for ($i = 0; $i < 100; $i++) {
            $key = $db->initKey("test", "demo", "key".$i);
            $status = $db->touch($key, 1000, array(Aerospike::OPT_WRITE_TIMEOUT=>200));
            if ($status != Aerospike::OK) {
                echo "Multitouch failed at ".$i."th record";
                break;
            }
        }
        $db->close();
    } else {
        echo "Aerospike DB connection is not established";
    }
});

$app->get('/getmany', function() use($db) {
    if ($db->isConnected()) {
	    $keys=[];
        for ($i = 0 ;$i < 100; $i++) {
            $keys[] = $db->initKey("test", "demo", "key".$i);
        }
        $status = $db->getMany($keys, $records);
        var_dump($records);
        if ($status != Aerospike::OK) {
            echo "\ngetMany operation failed!";
        }
	    $db->close();
    } else {
        echo "Aerospike DB connection is not established";
    }
});

$app->get('/existsmany', function() use($db) {
    if ($db->isConnected()) {
	    $keys=[];
        for ($i = 0 ;$i < 100; $i++) {
            $keys[] = $db->initKey("test", "demo", "key".$i);
        }
        $status = $db->existsMany($keys, $meta);
        var_dump($meta);
        if ($status != Aerospike::OK) {
            echo "\nexistsMany operation failed!";
        }
	    $db->close();
    } else {
        echo "Aerospike DB connection is not established";
    }
});

$app->get('/scan', function() use($db) {
    if ($db->isConnected()) {
        $status = $db->scan("test", "demo", function ($record) {
            var_dump($record);
            return true;
        }, array(Aerospike::OPT_SCAN_CONCURRENTLY=>true));
            var_dump($db->error());
        if ($status != Aerospike::OK) {
            echo "Scan Failed";
        }
        $db->close();
    } else {
        echo "Aerospike DB connection is not established";
    }
});

$app->get('/scanApply', function() use($db, $LUA_DIR) {
    if ($db->isConnected()) {
        $status = $db->register($LUA_DIR . "/test_record_udf.lua", "test_record_udf.lua");
        if ($status != Aerospike::OK) {
            echo "UDF register failed\n";
            $db->close();
            exit(0);
        }
        $status = $db->scanApply("test", "demo", "test_record_udf", "bin_udf_operation_integer", array('age', 1, 2), $scan_id);
        if ($status != Aerospike::OK) {
            echo "Scan Apply Failed";
            break;
        }

        $status = $db->scanInfo($scan_id, $scan_info);
        if ($status != Aerospike::OK) {
            echo "Scan Info Failed";
            break;
        }
        var_dump($scan_info);
        $db->close();
    } else {
        echo "Aerospike DB connection is not established";
    }
});

$app->get('/query', function() use($db) {
    if ($db->isConnected()) {
        $key = $db->initKey("test", "demo", "key12");
        $db->put($key,array("first_name"=>"john", "age"=>22));
        $key = $db->initKey("test", "demo", "key15");
        $db->put($key,array("first_name"=>"john", "age"=>25));

        $return_value_index = $db->addIndex("test", "demo", "age", "age_dx", Aerospike::INDEX_TYPE_DEFAULT, Aerospike::INDEX_NUMERIC);
        $where = $db->predicateEquals("age", 25);

        $status = $db->query("test", "demo", $where, function ($record) {
            var_dump($record);
            return true;
        });
        if ($status != Aerospike::OK) {
            var_dump("Query Failed");
            var_dump($db->error());
            var_dump($db->errorno());
        } else {
            var_dump("Query successful");
            var_dump($status);
        }
        $db->close();
    } else {
        echo "Aerospike DB connection is not established";
    }
});

$app->get('/register', function() use($db, $LUA_DIR) {
    if ($db->isConnected()) {
        $status = $db->register($LUA_DIR . "/test_record_udf.lua", "test_record_udf.lua");
        if ($status != Aerospike::OK) {
            var_dump($db->error());
            var_dump("UDF register failed");
        } else {
            var_dump("UDF registered successfully");
        }
        $db->close();
    } else {
        echo "Aerospike DB connection is not established";
    }
});

$app->get('/listRegistered', function() use($db) {
    if ($db->isConnected()) {
        $status = $db->listRegistered($modules);
        if ($status != Aerospike::OK) {
            var_dump("listRegistered operation failed");
        } else {
            echo "Registered modules to Aerospike DB are : \n";
            var_dump($modules);
        }
        $db->close();
    } else {
        echo "Aerospike DB connection is not established";
    }
});

$app->get('/getRegistered', function() use($db) {
    if ($db->isConnected()) {
        $status = $db->getRegistered("test_record_udf.lua", $module_code);
        if ($status != Aerospike::OK) {
            var_dump("getRegistered operation failed");
        } else {
            echo "getRegistered successful\n";
            //var_dump($module_code);
        }
        $db->close();
    } else {
        echo "Aerospike DB connection is not established";
    }
});

$app->get('/apply', function() use($db) {
    if ($db->isConnected()) {
        $key = $db->initKey("test", "demo", "integer_key");
        $record = array("name" => "name100", "age" => 100);
        $status = $db->put($key, $record);
        $status = $db->apply($key, "test_record_udf", "bin_udf_operation_integer", array("age", 2, 20), $returned);
        if ($status != Aerospike::OK) {
            var_dump("udf_apply operation failed");
        } else {
            echo "UDF apply result : ";
            var_dump($returned);
        }
        $db->remove($key);
        $db->close();
    } else {
        echo "Aerospike DB connection is not established";
    }
});

$app->get('/deregister', function() use($db) {
    if ($db->isConnected()) {
        $status = $db->deregister("test_record_udf.lua");
        if ($status != Aerospike::OK) {
            var_dump("UDF deregister failed");
        } else {
            var_dump("UDF deregistered successfully");
        }
        $db->close();
    } else {
        echo "Aerospike DB connection is not established";
    }
});

$app->put('/removebin', function() use($db) {
    if ($db->isConnected()) {
        for ($i = 0; $i < 100; $i++) {
            $key = $db->initKey("test", "demo", "key".$i);
            $status = $db->removeBin($key, array("age"));
            if ($status != Aerospike::OK) {
                echo "RemoveBin failed at ".$i."th record";
                break;
            }
        }
        $db->close();
    } else {
        echo "Aerospike DB connection is not established";
    }
});

$app->put('/remove', function() use($db) {
    if ($db->isConnected()) {
        for ($i = 0; $i < 100; $i++) {
            $key = $db->initKey("test", "demo", "key".$i);
            $status = $db->remove($key);
            if ($status != Aerospike::OK) {
                echo "Remove failed at ".$i."th record";
                break;
            }
        }
        $db->close();
    } else {
        echo "Aerospike DB connection is not established";
    }
});

$app->put('/closeconnection', function() use($db) {
    $status = $db->close();
    if ($status != Aerospike::OK) {
        echo "Connection is already closed";
        exit(1);
    }
    echo "Connection to the Aerospike database closed";
});

$app->get('/reconnect', function() use($db) {
    $status = $db->reconnect();
    if ($status != Aerospike::OK) {
        echo "Already connected to the Aerospike DB";
        exit(1);
    }
    echo "Connection re-established to the Aerospike database";
});

$app->run();
?>
