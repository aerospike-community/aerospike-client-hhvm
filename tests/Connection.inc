<?php

require_once 'Common.inc';
/**
 *Basic connection tests
*/

class Connection extends AerospikeTestCommon
{

    protected function setUp() {
        $config = array("hosts"=>array(array("addr"=>AEROSPIKE_CONFIG_NAME, "port"=>AEROSPIKE_CONFIG_PORT)));
        $this->db = new Aerospike($config);
        if (!$this->db->isConnected()) {
            return $this->db->errorno();
        }
    }

    /**
     * @test
     * Basic Connection Test
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testConnect)
     *
     * @test_plans{1.1}
     */
    function testConnect() {
        $status = $this->db->errorno();
        return $status;
    }
    /**
     * @test
     * Config Argument Not An Array
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testConfigNotArray)
     *
     * @test_plans{1.1}
     */
    function testConfigNotArray() {
        try {
            $config = "";
            $db = new Aerospike($config);
            return($db->errorno());
        } catch (ErrorException $e) {
            return Aerospike::ERR_PARAM;
        }
    }
    /**
     * @test
     * Config Argument Is An Empty Array
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testEmptyArray)
     *
     * @test_plans{1.1}
     */
    function testEmptyArray() {
        $config = array("hosts"=>array(array("addr"=>"", "port"=>"")));
        $db = new Aerospike($config);
        return($db->errorno());
    }
    /**
     * @test
     * Config Array Missing The Host Key
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testMissingHostKeyFromConfigArray)
     *
     * @test_plans{1.1}
     */
    function testMissingHostsKeyFromConfigArray() {
        $config = array(array(array("addr"=>AEROSPIKE_CONFIG_NAME, "port"=>AEROSPIKE_CONFIG_PORT)));
        $db = new Aerospike($config);
        return ($db->errorno());
    }
    /**
     * @test
     * Config Array Missing Addr Key
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testMissingAddrKeyFromConfigArray)
     *
     * @test_plans{1.1}
     */
    function testMissingAddrKeyFromConfigArray() {
        $config = array("hosts"=>array(array("0"=>AEROSPIKE_CONFIG_NAME, "port"=>AEROSPIKE_CONFIG_PORT)));
        $db = new Aerospike($config);
        return ($db->errorno());
    }
    /**
     * @test
     * Config Array Missing Port Key
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testMissingPortKeyFromConfigArray)
     *
     * @test_plans{1.1}
     */
    function testMissingPortKeyFromConfigArray() {
        $config = array("hosts"=>array(array("addr"=>AEROSPIKE_CONFIG_NAME, "qw"=>AEROSPIKE_CONFIG_PORT)));
        $db = new Aerospike($config);
        return ($db->errorno());
    }
    /**
     * @test
     * Multiple Hosts
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testMultipleHostsArray)
     *
     * @test_plans{1.1}
     */
    function testMultipleHostsArray() {
        $config = array("hosts"=>array(
                    array("addr"=>AEROSPIKE_CONFIG_NAME,"port"=>AEROSPIKE_CONFIG_PORT),
                    array("addr"=>AEROSPIKE_CONFIG_NAME2,"port"=>AEROSPIKE_CONFIG_PORT2)));
        $db = new Aerospike($config);
        $status = $db->errorno();
        if($db->isConnected()) {
        $db->close();
        return ($status);
        }
        return Aerospike::ERR_CLIENT;
    }
    /**
     * @test
     * Port value is string
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testPortValueIsString)
     *
     * @test_plans{1.1}
     */
    function testPortValueIsString() {
        $config = array("hosts"=>array(array("addr"=>AEROSPIKE_CONFIG_NAME, "port"=>"3000")));
        $db = new Aerospike($config);
        $status = $db->errorno();
        if($db->isConnected()) {
            $db->close();
            return $status;
        }
        return Aerospike::ERR_CLIENT;
    }
    /**
     * @test
     * Test With Null Persistence Alias
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testNullAlias)
     *
     * @test_plans{1.1}
     */
    function testNullAlias() {
        $config = array("hosts"=>array(array("port"=>AEROSPIKE_CONFIG_PORT,"addr"=>AEROSPIKE_CONFIG_NAME)));
        $db = new Aerospike($config, NULL);
        $status = $db->errorno();
        if($db->isConnected()) {
        $db->close();
        return $status;
        }
        return Aerospike::ERR_CLIENT;
    }
    /**
     * @test
     * Alias not passed and different config
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testNamePortSequenceInConfig)
     *
     * @test_plans{1.1}
     */
    function testNoAliasDiffrentConfig() {
        $test1 = false;
        $test2 = false;
        $config1 = array("hosts"=>array(array("addr"=>AEROSPIKE_CONFIG_NAME,"port"=>AEROSPIKE_CONFIG_PORT)));
        $db1 = new Aerospike($config1);
        $config2 = array("hosts"=>array(array("addr"=>AEROSPIKE_CONFIG_NAME2,"port"=>AEROSPIKE_CONFIG_PORT2)));
        $db2 = new Aerospike($config2);
        if ($db1->isConnected()) {
            $test1 = true;
            $db1->close();
        }
        if ($db2->isConnected()) {
            $test2 = true;
            $db2->close();
        }
        if ($test1 && $test2) {
            return Aerospike::OK;
        } elseif (!$test1) {
            return $db1->errorno();
        } else {
            return $db2->errorno();
        }
    }


    /**
     * @test
     * Basic Connection
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testConnect)
     *
     * @test_plans{1.1}
     */
    function testConnectWithOption() {
        $config = array("hosts"=>array(array("addr"=>AEROSPIKE_CONFIG_NAME, "port"=>AEROSPIKE_CONFIG_PORT)));
        $opts = array(Aerospike::OPT_CONNECT_TIMEOUT => 1250,
            Aerospike::OPT_WRITE_TIMEOUT => 1500);
        $db = new Aerospike($config, NULL, $opts);
        $status = $db->errorno();
        if($db->isConnected()) {
        $db->close();
        return ($status);
        }
        return Aerospike::ERR_CLIENT;
    }

    function testConnectWithPersistFlagFalse() {
        $config = array("hosts"=>array(array("addr"=>AEROSPIKE_CONFIG_NAME,
            "port"=>AEROSPIKE_CONFIG_PORT)));
        $db = new Aerospike($config, false);
        $status = $db->errorno();
        if($db->isConnected()) {
        $db->close();
        return ($status);
        }
        return Aerospike::ERR_CLIENT;
    }

    function testConnectWithPersistFlagPassedAsStr() {
        $config = array("hosts"=>array(array("addr"=>AEROSPIKE_CONFIG_NAME,
            "port"=>AEROSPIKE_CONFIG_PORT)));
        $db = new Aerospike($config, "persist");
        $status = $db->errorno();
        if($db->isConnected()) {
        $db->close();
        return ($status);
        }
        return Aerospike::ERR_CLIENT;
    }

    function testConnectWithPersistFlagPassedAsInt() {
        $config = array("hosts"=>array(array("addr"=>AEROSPIKE_CONFIG_NAME,
            "port"=>AEROSPIKE_CONFIG_PORT)));
        $db = new Aerospike($config, 11111222);
        $status = $db->errorno();
        if($db->isConnected()) {
        $db->close();
        return ($status);
        }
        return Aerospike::ERR_CLIENT;
    }

    function testConnectWithPersistFlagTrue() {
        $config = array("hosts"=>array(array("addr"=>AEROSPIKE_CONFIG_NAME,
            "port"=>AEROSPIKE_CONFIG_PORT)));
        $db = new Aerospike($config, true);
        $status = $db->errorno();
        if($db->isConnected()) {
        $db->close();
        return ($status);
        }
        return Aerospike::ERR_CLIENT;
    }

    function testConnectWithSameConfigPersistAsTrue() {
        $config = array("hosts"=>array(array("addr"=>AEROSPIKE_CONFIG_NAME,
            "port"=>AEROSPIKE_CONFIG_PORT)));
        $config1 = array("hosts"=>array(array("addr"=>AEROSPIKE_CONFIG_NAME,
            "port"=>AEROSPIKE_CONFIG_PORT)));
        $db = new Aerospike($config, true);
        if (!$db->isConnected()) {
            $db->close();
            return Aerospike::ERR_CLIENT;
        }
        $db1 = new Aerospike($config1, true);
        if (!$db1->isConnected()) {
            $db1->close();
            return Aerospike::OK;
        }
        $db1->close();
        $db->close();
        return Aerospike::OK;
    }
} 
?>
