<?php
/**
 * Copyright 2013-2015 Aerospike, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @category   Database
 * @author     Ronen Botzer <rbotzer@aerospike.com>
 * @copyright  Copyright 2013-2015 Aerospike, Inc.
 * @license    http://www.apache.org/licenses/LICENSE-2.0 Apache License, Version 2
 * @link       http://www.aerospike.com/docs/guide/lmap.html
 * @filesource
 * @deprecated
 */
namespace Aerospike\LDT;
use Aerospike;

/**
 * Large Map (lmap) is a large data type that employs the standard "key/value"
 * map functionality. It is optimized for complex values where the key
 * may be an integer or string, and the value may be any supported data type.
 *
 * @package    Aerospike
 * @subpackage LDT
 * @link       http://www.aerospike.com/docs/guide/lmap.html
 * @link       http://www.aerospike.com/docs/guide/data-types.html
 * @author     Ronen Botzer <rbotzer@aerospike.com>
 * @deprecated
 */
class LMap extends LDT
{
    // error messages
    const MSG_TYPE_NOT_SUPPORTED = "\$value must be a supported type (string|integer|array)";
    const MSG_TYPE_NOT_ATOMIC = "\$key must be an atomic type (string|integer)";

    /**
     * Constructor for the \Aerospike\LDT\LMap class.
     *
     * @param Aerospike $db
     * @param array $key initialized with Aerospike::initKey()
     * @param string $bin name
     * @see LDT::__construct()
     * @deprecated
     */
    public function __construct(Aerospike $db, array $key, $bin) {
        parent::__construct($db, $key, $bin, LDT::LMAP);
    }

    /**
     * Puts the key-value pair in the LMap.
     *
     * @param int|string $key
     * @param int|string|array $value
     * @return int status code of the operation
     * @deprecated
     */
    public function put($key, $value) {
        if (!is_string($key) && !is_int($key)) {
            $this->errorno = self::ERR_INPUT_PARAM;
            $this->error = self::MSG_TYPE_NOT_ATOMIC;
            return $this->errorno;
        }
        if (!is_string($value) && !is_int($value) && !is_array($value)) {
            $this->errorno = self::ERR_INPUT_PARAM;
            $this->error = self::MSG_TYPE_NOT_SUPPORTED;
            return $this->errorno;
        }
        $status = $this->db->apply($this->key, 'lmap', 'put', array($this->bin, $key, $value));
        $this->processStatusCode($status);
        return $this->errorno;
    }

    /**
     * Puts multiple key-value pairs in the LMap.
     * Each one of the elements added must conform to the same type rules as in
     * the {put()} method.
     *
     * @param array $key_values
     * @return int status code of the operation
     * @see put
     * @deprecated
     */
    public function putMany(array $key_values) {
        $status = $this->db->apply($this->key, 'lmap', 'put_all', array($this->bin, $key_values));
        $this->processStatusCode($status);
        return $this->errorno;
    }

    /**
     * Gets the element matching the given key in the LMap.
     *
     * @param int|string $key
     * @param array $element matched
     * @return int status code of the operation
     * @deprecated
     */
    public function get($key, &$element) {
        if (!is_string($key) && !is_int($key)) {
            $this->errorno = self::ERR_INPUT_PARAM;
            $this->error = self::MSG_TYPE_NOT_ATOMIC;
            return $this->errorno;
        }
        $element = array();
        $status = $this->db->apply($this->key, 'lmap', 'get', array($this->bin, $key), $element);
        $this->processStatusCode($status);
        return $this->errorno;
    }

    /**
     * Remove an element matching the given key in the LMap.
     *
     * @param int|string $key
     * @return int status code of the operation
     * @deprecated
     */
    public function remove($key) {
        if (!is_string($key) && !is_int($key)) {
            $this->errorno = self::ERR_INPUT_PARAM;
            $this->error = self::MSG_TYPE_NOT_ATOMIC;
            return $this->errorno;
        }
        $status = $this->db->apply($this->key, 'lmap', 'remove', array($this->bin, $key));
        $this->processStatusCode($status);
        return $this->errorno;
    }

    /**
     * Returns the elements in the LMap.
     *
     * @param array $elements returned
     * @return int status code of the operation
     * @deprecated
     */
    public function scan(&$elements) {
        $elements = array();
        $status = $this->db->apply($this->key, 'lmap', 'scan', array($this->bin), $elements);
        $this->processStatusCode($status);
        return $this->errorno;
    }

    /**
     * Scan the LMap and apply a UDF to its elements.
     * If the UDF returns null the element will be filtered out of the results.
     * Otherwise, the UDF may transform the value of the element prior to
     * returning it to the result set.
     *
     * @todo To be implemented.
     * @param string $module name of the UDF library
     * @param string $function name of the UDF
     * @param array $args passed to the UDF
     * @param array $elements returned
     * @return int status code of the operation
     * @deprecated
     */
    public function filter($module, $function, array $args, array &$elements) {
        $this->error = "Method not implemented";
        $this->errorno = self::ERR_LDT;
        return $this->errorno;
    }

}

?>
