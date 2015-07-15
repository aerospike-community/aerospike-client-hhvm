<?php
require_once 'Common.inc';

/**
 *Basic Index creation and drop tests
*/

class Index extends AerospikeTestCommon
{
    function setUp() {
        $config = array("hosts"=>array(array("addr"=>AEROSPIKE_CONFIG_NAME, "port"=>AEROSPIKE_CONFIG_PORT)));
        $this->db = new Aerospike($config);
        $key = $this->db->initKey("test", "demo", "key1");
        $this->db->put($key, array("bin1"=>"1", "bin2"=>2, "bin3"=>3, "numeric_list"=>array(1,2,3)));
        $this->keys[] = $key;
        $key = $this->db->initKey("test", "demo", "key2");
        $this->db->put($key, array("string_list"=>array('a', 'b', 'c', 'd', 'e')));
        $this->keys[] = $key;
        $key = $this->db->initKey("test", "demo", "key3");
		$this->db->put($key, array("string_map_keys"=>array('a' =>1, 'b' => 2, 'c' => 3),
			"string_map_values"=>array('a' => 'x', 'b' => 'y', 'c' => 'z')));
        $this->keys[] = $key;
        $key = $this->db->initKey("test", "demo", "key4");
		$this->db->put($key, array("num_mapkeys"=>array(1 => 'aa', 2 => 'bb', 3 => 'cc'),
			"numeric_map_values"=>array('a' => 11, 'b' => 12, 'c' => 13)));
        $this->keys[] = $key;
   }


    /**
     * @test
     * Negative index drop operation on integer bin: drop a non existing index
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testDropIndexIntegerBinInvalidIndexNegative)
     *
     * @test_plans{1.1}
     */
    function testDropIndexIntegerBinInvalidIndexNegative() {
        return($this->db->dropIndex("test", "invalid_index"));
    }

    /**
     * @test
     * Basic index create on list and datatype is string.
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testAddIndexListStringPositive)
     *
     * @test_plans{1.1}
     */
    function testAddIndexListStringPositive() {
        $status = $this->db->addIndex("test", "demo", "string_list",
			"index_list_string", Aerospike::INDEX_TYPE_LIST,
			Aerospike::INDEX_STRING, array(Aerospike::OPT_WRITE_TIMEOUT => 12000));
		if ($status != AEROSPIKE::OK) {
			return $this->db->errorno();
		}
        $status = $this->db->dropIndex("test", "index_list_string");
		if ($status != AEROSPIKE::OK) {
			return $this->db->errorno();
		}
		return $status;
    }
    /**
     * @test
     * Basic index create on list and datatype is integer.
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testAddIndexListNumericPositive)
     *
     * @test_plans{1.1}
     */
    function testAddIndexListNumericPositive() {
        $status = $this->db->addIndex("test", "demo", "numeric_list",
			"index_list_numeric", Aerospike::INDEX_TYPE_LIST,
			Aerospike::INDEX_NUMERIC, array(Aerospike::OPT_WRITE_TIMEOUT => 12000));
		if ($status != AEROSPIKE::OK) {
			return $this->db->errorno();
		}
        $status = $this->db->dropIndex("test", "index_list_numeric");
		if ($status != AEROSPIKE::OK) {
			return $this->db->errorno();
		}
		return $status;
    }
    /**
     * @test
     * Basic index create on map keys and datatype is string.
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testAddIndexMapKeysStringPositive)
     *
     * @test_plans{1.1}
     */
    function testAddIndexMapKeysStringPositive() {
        $status = $this->db->addIndex("test", "demo", "string_map_keys",
			"index_mapkeys_string", Aerospike::INDEX_TYPE_MAPKEYS,
			Aerospike::INDEX_STRING, array(Aerospike::OPT_WRITE_TIMEOUT => 12000));
		if ($status != AEROSPIKE::OK) {
			return $this->db->errorno();
		}
        $status = $this->db->dropIndex("test", "index_mapkeys_string");
		if ($status != AEROSPIKE::OK) {
			return $this->db->errorno();
		}
		return $status;
    }
    /**
     * @test
     * Basic index create on map keys and datatype is numeric.
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testAddIndexMapKeysNumericPositive)
     *
     * @test_plans{1.1}
     */
    function testAddIndexMapKeysNumericPositive() {
        $status = $this->db->addIndex("test", "demo", "nume_mapkeys",
			"index_mapkeys_numeric", Aerospike::INDEX_TYPE_MAPKEYS,
			Aerospike::INDEX_NUMERIC, array(Aerospike::OPT_WRITE_TIMEOUT => 12000));
		if ($status != AEROSPIKE::OK) {
			return $this->db->errorno();
		}
        $status = $this->db->dropIndex("test", "index_mapkeys_numeric");
		if ($status != AEROSPIKE::OK) {
			return $this->db->errorno();
		}
		return $status;
    }
    /**
     * @test
     * Basic index create on map values and datatype is string.
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testAddIndexMapValuesStringPositive)
     *
     * @test_plans{1.1}
     */
    function testAddIndexMapValuesStringPositive() {
        $status = $this->db->addIndex("test", "demo", "string_map_values",
			"index_mapvalues_string", Aerospike::INDEX_TYPE_MAPVALUES,
			Aerospike::INDEX_STRING, array(Aerospike::OPT_WRITE_TIMEOUT => 12000));
		if ($status != AEROSPIKE::OK) {
			return $this->db->errorno();
		}
        $status = $this->db->dropIndex("test", "index_mapvalues_string");
		if ($status != AEROSPIKE::OK) {
			return $this->db->errorno();
		}
		return $status;
    }
    /**
     * @test
     * Basic index create on map values and datatype is numeric.
     *
     * @pre
     * Connect using aerospike object to the specified node
     *
     * @post
     * newly initialized Aerospike objects
     *
     * @remark
     * Variants: OO (testAddIndexMapValuesNumericPositive)
     *
     * @test_plans{1.1}
     */
    function testAddIndexMapValuesNumericPositive() {
        $status = $this->db->addIndex("test", "demo", "numeric_map_values",
			"index_mapvalues_numeric", Aerospike::INDEX_TYPE_MAPKVALUES,
			Aerospike::INDEX_NUMERIC, array(Aerospike::OPT_WRITE_TIMEOUT => 12000));
		if ($status != AEROSPIKE::OK) {
			return $this->db->errorno();
		}
        $status = $this->db->dropIndex("test", "index_mapvalues_numeric");
		if ($status != AEROSPIKE::OK) {
			return $this->db->errorno();
		}
		return $status;
    }
}
?>
