<?php
require_once 'Common.inc';

/**
 *Basic remove opeartion tests
*/

class Remove extends AerospikeTestCommon
{
    protected function setUp() {
        $config = array("hosts"=>array(array("addr"=>AEROSPIKE_CONFIG_NAME, "port"=>AEROSPIKE_CONFIG_PORT)));
        $this->db = new Aerospike($config);
        if (!$this->db->isConnected()) {
            return $this->db->errorno();
        }
        $key = $this->db->initKey("test", "demo", "remove_test");
        $this->db->put($key,array("bin1"=>"1"));
        $this->keys[] = $key;
    }

    /**
     * @test
     * Basic key remove operation
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testKeyRemove)
     *
     * @test_plans{1.1}
     */
    function testKeyRemove() {
        $this->db->remove($this->keys[0]);
        $status = $this->db->get($this->keys[0], $get_record, array('bin1'));
        return $status;
    }
    /**
     * @test
     * Basic key remove operation with policy
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testKeyRemove)
     *
     * @test_plans{1.1}
     */
    function testKeyRemoveWithPolicy() {
        $this->db->remove($this->keys[0],
            array(Aerospike::OPT_POLICY_RETRY =>Aerospike::POLICY_RETRY_NONE,
            Aerospike::OPT_POLICY_KEY=>Aerospike::POLICY_KEY_SEND,
            Aerospike::OPT_POLICY_COMMIT_LEVEL=>Aerospike::POLICY_COMMIT_LEVEL_MASTER));
        $status = $this->db->get($this->keys[0], $get_record, array('bin1'));
        return $status;
    }
    /**
     * @test
     * Basic key remove operation when key not exist
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testKeyRemoveKeyNotExist)
     *
     * @test_plans{1.1}
     */
    function testKeyRemoveKeyNotExist() {
        $key1 = array("ns" => "test", "set" => "demo", "key" => "-----ss-----");
        return ($this->db->remove($key1, array(Aerospike::OPT_POLICY_RETRY => Aerospike::POLICY_RETRY_NONE,
                Aerospike::OPT_POLICY_KEY=>Aerospike::POLICY_KEY_DIGEST)));
    }
    /**
     * @test
     * Basic key remove operation with policy gen GT lesser
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testKeyRemove)
     *
     * @test_plans{1.1}
     */
    function testKeyRemoveWithGenGTLesser() {
        $exists_status = $this->db->exists($this->keys[0], $metadata);
        $gen_value = $metadata["generation"];
        return ($this->db->remove($this->keys[0],
            array(Aerospike::OPT_POLICY_GEN=>array(Aerospike::POLICY_GEN_GT,
            $gen_value))));
    }
    /**
     * @test
     * Basic key remove operation with policy gen GT positive
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testKeyRemove)
     *
     * @test_plans{1.1}
     */
    function testKeyRemoveWithGenGTPositive() {
        $exists_status = $this->db->exists($this->keys[0], $metadata);
        $gen_value = $metadata["generation"] + 10;

        $this->db->remove($this->keys[0], array(Aerospike::OPT_POLICY_GEN=>array(Aerospike::POLICY_GEN_GT, $gen_value)));
        $status = $this->db->get($this->keys[0], $get_record, array('bin1'));
        return $status;
    }
    /**
     * @test
     * Basic key remove operation with policy gen EQ positive
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testKeyRemove)
     *
     * @test_plans{1.1}
     */
    function testKeyRemoveWithGenEQPositive() {
        $exists_status = $this->db->exists($this->keys[0], $metadata);
        $gen_value = $metadata["generation"];

        $this->db->remove($this->keys[0], array(Aerospike::OPT_POLICY_GEN=>array(Aerospike::POLICY_GEN_EQ, $gen_value)));
        $status = $this->db->get($this->keys[0], $get_record, array('bin1'));
        return $status;
    }
    /**
     * @test
     * Basic key remove operation with policy gen EQ not equal
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testKeyRemove)
     *
     * @test_plans{1.1}
     */
    function testKeyRemoveWithGenEQNotEqual() {
        $exists_status = $this->db->exists($this->keys[0], $metadata);
        $gen_value = $metadata["generation"] + 5;
        $check = array("bin1"=>"1");
        return ($this->db->remove($this->keys[0],
            array(Aerospike::OPT_POLICY_GEN=>array(Aerospike::POLICY_GEN_EQ,
            $gen_value))));
    }
    /**
     * @test
     * Basic key remove operation with policy gen ignore
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testKeyRemove)
     *
     * @test_plans{1.1}
     */
    function testKeyRemoveWithGenIgnore() {
        $exists_status = $this->db->exists($this->keys[0], $metadata);
        $gen_value = $metadata["generation"];

        $this->db->remove($this->keys[0], array(Aerospike::OPT_POLICY_GEN=>array(Aerospike::POLICY_GEN_IGNORE, $gen_value)));
        $status = $this->db->get($this->keys[0], $get_record, array('bin1'));
        return $status;
    }
    /**
     * @test
     * Basic key remove operation with policy key send
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testKeyRemove)
     *
     * @test_plans{1.1}
     */
    function testKeyRemoveWithPolicyKeySend() {
        $put_status = $this->db->put($this->keys[0], array("bin1"=>"1"), NULL,
         array(Aerospike::OPT_POLICY_KEY=>Aerospike::POLICY_KEY_SEND));
        $this->db->remove($this->keys[0], array(Aerospike::OPT_POLICY_KEY=>Aerospike::POLICY_KEY_SEND));
        $status = $this->db->get($this->keys[0], $get_record, array('bin1'));
        return $status;
    }
    /**
     * @test
     * Basic key remove operation with policy key digest
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testKeyRemove)
     *
     * @test_plans{1.1}
     */
    function testKeyRemoveWithPolicyKeyDigest() {
         $key = $this->db->initKey("test", "demo",
         base64_decode("removebin_policy_key_digest"), true);
        $put_status = $this->db->put($key, array("bin1"=>"1"), NULL,
         array(Aerospike::OPT_POLICY_KEY=>Aerospike::POLICY_KEY_DIGEST));
        $this->db->remove($key, array(Aerospike::OPT_POLICY_KEY=>Aerospike::POLICY_KEY_DIGEST));
        $status = $this->db->get($key, $get_record, array('bin1'));
        return $status;
    }

    /**
     * @test
     * Basic bin remove operation
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testBinRemove)
     *
     * @test_plans{1.1}
     */
    function testBinRemove() {
        $this->db->removeBin($this->keys[0], array("bin1"));
        $status = $this->db->get($this->keys[0], $get_record, array('bin1'));
        if(!array_key_exists('bins', $get_record)) {
            return $status;
        }
        return Aerospike::ERR_CLIENT;
    }

    /**
     * @test
     * Basic bin remove operation with policy
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testBinRemove)
     *
     * @test_plans{1.1}
     */
    function testBinRemoveWithPolicy() {
        $this->db->removeBin($this->keys[0], array("bin1"), array(Aerospike::OPT_WRITE_TIMEOUT=>1000));
        $status = $this->db->get($this->keys[0], $get_record, array('bin1'));
        if(!array_key_exists('bins', $get_record)) {
            return $status;
        }
        return Aerospike::ERR_CLIENT;
    }
    /**
     * @test
     * Basic bin remove operation with options
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testBinRemove)
     *
     * @test_plans{1.1}
     */
    function testBinRemoveWithOptions() {
        if ($this->db->isConnected()) {
            return $this->db->removeBin($this->keys[0], array("bin1"),
                array(Aerospike::OPT_POLICY_COMMIT_LEVEL=>Aerospike::POLICY_COMMIT_LEVEL_MASTER));
        } else {
            return($this->db->errorno());
        }
    }
    /**
     * @test
     * Basic key remove operation when key not exist
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testBinRemoveBinNotExist)
     *
     * @test_plans{1.1}
     */
    function testBinRemoveBinNotExist() {
        return($this->db->removeBin($this->keys[0], array("-------------bin1------------")));
    }
    /**
     * @test
     * Basic bin remove operation with policy gen GT lesser
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testBinRemove)
     *
     * @test_plans{1.1}
     */
    function testRemoveBinWithGenGTLesser() {
        $exists_status = $this->db->exists($this->keys[0], $metadata);
        $gen_value = $metadata["generation"];
        $check = array("bin1"=>"1");
        $this->db->removeBin($this->keys[0], array("bin1"), array(Aerospike::OPT_POLICY_GEN=>array(Aerospike::POLICY_GEN_GT, $gen_value)));
        $status = $this->db->get($this->keys[0], $get_record, array('bin1'));
        $comp_res = array_diff_assoc_recursive($get_record, $check);
        if (!empty($comp_res)) {
            return Aerospike::ERR_CLIENT;
        }
        else {
            return AEROSPIKE::OK;
        }
    }
    /**
     * @test
     * Basic bin remove operation with policy gen GT positive
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testBinRemove)
     *
     * @test_plans{1.1}
     */
    function testRemoveBinWithGenGTPositive() {
        $exists_status = $this->db->exists($this->keys[0], $metadata);
        $gen_value = $metadata["generation"] + 10;

        $this->db->removeBin($this->keys[0], array("bin1"), array(Aerospike::OPT_POLICY_GEN=>array(Aerospike::POLICY_GEN_GT, $gen_value)));
        $status = $this->db->get($this->keys[0], $get_record, array('bin1'));
        if(!array_key_exists('bins', $get_record)) {
            return $status;
        }
        return Aerospike::ERR_CLIENT;

    }
    /**
     * @test
     * Basic bin remove operation with policy gen EQ positive
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testBinRemove)
     *
     * @test_plans{1.1}
     */
    function testRemoveBinWithGenEQPositive() {
        $exists_status = $this->db->exists($this->keys[0], $metadata);
        $gen_value = $metadata["generation"];
        $this->db->removeBin($this->keys[0], array("bin1"), array(Aerospike::OPT_POLICY_GEN=>array(Aerospike::POLICY_GEN_EQ, $gen_value)));
        $status = $this->db->get($this->keys[0], $get_record, array('bin1'));
        if(!array_key_exists('bins',$get_record)) {
            return $status;
        }
        return Aerospike::ERR_CLIENT;

    }
    /**
     * @test
     * Basic bin remove operation with policy gen EQ not equal
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testBinRemove)
     *
     * @test_plans{1.1}
     */
    function testRemoveBinWithGenEQNotEqual() {
        $exists_status = $this->db->exists($this->keys[0], $metadata);
        $gen_value = $metadata["generation"] + 5;
        $check = array("bin1"=>"1");
        var_dump($this->db->removeBin($this->keys[0], array("bin1"), array(Aerospike::OPT_POLICY_GEN=>array(Aerospike::POLICY_GEN_EQ, $gen_value))));
        $status = $this->db->get($this->keys[0], $get_record, array('bin1'));
        var_dump($get_record, $check);
        $comp_res = array_diff_assoc_recursive($get_record, $check);
        if (!empty($comp_res)) {
            return Aerospike::ERR_CLIENT;
        }
        else {
            return AEROSPIKE::OK;
        }
    }
    /**
     * @test
     * Basic bin remove operation with policy gen ignore
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testBinRemove)
     *
     * @test_plans{1.1}
     */
    function testRemoveBinWithGenIgnore() {
        $exists_status = $this->db->exists($this->keys[0], $metadata);
        $gen_value = $metadata["generation"];

        $this->db->removeBin($this->keys[0], array("bin1"), array(Aerospike::OPT_POLICY_GEN=>array(Aerospike::POLICY_GEN_IGNORE, $gen_value)));
        $status = $this->db->get($this->keys[0], $get_record, array('bin1'));
        if(!array_key_exists('bins',$get_record)) {
            return $status;
        }
        return Aerospike::ERR_CLIENT;

    }
    /**
     * @test
     * Basic bin remove operation with policy key send
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testBinRemove)
     *
     * @test_plans{1.1}
     */
    function testRemoveBinWithPolicyKeySend() {
        $put_status = $this->db->put($this->keys[0], array("bin1"=>"1"), NULL,
         array(Aerospike::OPT_POLICY_KEY=>Aerospike::POLICY_KEY_SEND));

        $this->db->removeBin($this->keys[0], array("bin1"), array(Aerospike::OPT_POLICY_KEY=>Aerospike::POLICY_KEY_SEND));
        $status = $this->db->get($this->keys[0], $get_record, array('bin1'));
        if(!array_key_exists('bins',$get_record)) {
            return $status;
        }
        return Aerospike::ERR_CLIENT;
    }
    /**
     * @test
     * Basic bin remove operation with policy key digest
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testBinRemove)
     *
     * @test_plans{1.1}
     */
    function testRemoveBinWithPolicyKeyDigest() {
         $key = $this->db->initKey("test", "demo",
         base64_decode("removebin_policy_key_digest"), true);
        $put_status = $this->db->put($key, array("bin1"=>"1"), NULL,
         array(Aerospike::OPT_POLICY_KEY=>Aerospike::POLICY_KEY_DIGEST));

        $this->db->removeBin($key, array("bin1"), array(Aerospike::OPT_POLICY_KEY=>Aerospike::POLICY_KEY_DIGEST));
        $status = $this->db->get($key, $get_record, array('bin1'));
        if(!array_key_exists('bins',$get_record)) {
            return $status;
        }
        return Aerospike::ERR_CLIENT;
    }
}
?>
