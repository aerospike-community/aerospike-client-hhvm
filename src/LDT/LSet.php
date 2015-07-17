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
 * @link       http://www.aerospike.com/docs/guide/lset.html
 * @filesource
 * @deprecated
 */
namespace Aerospike\LDT;
use Aerospike;

/**
 * Large Set (lset) is a large data type optimized for storing unique values
 * and checking the existance of a value in the set. Large sets are suited for
 * storing a large number of unique values.
 *
 * @package    Aerospike
 * @subpackage LDT
 * @link       http://www.aerospike.com/docs/guide/lset.html
 * @author     Ronen Botzer <rbotzer@aerospike.com>
 * @deprecated
 */
class LSet extends LDT
{
    // error messages
    const MSG_TYPE_NOT_SUPPORTED = "\$value must be a supported type (string|integer|array)";
    const MSG_TYPE_NOT_ATOMIC = "\$value must be an atomic type (string|integer)";

    /**
     * Constructor for the \Aerospike\LDT\LSet class.
     *
     * @param Aerospike $db
     * @param array $key initialized with Aerospike::initKey()
     * @param string $bin name
     * @see LDT::__construct()
     * @deprecated
     */
    public function __construct(Aerospike $db, array $key, $bin) {
        parent::__construct($db, $key, $bin, LDT::LSET);
    }

    /**
     * Adds a value of a supported type to the LSet.
     * The elements added must be consistently the same type
     * (string, integer, array).
     *
     * @param int|string|array $value
     * @return int status code of the operation
     * @deprecated
     */
    public function add($value) {
        if (!is_string($value) && !is_int($value) && !is_array($value)) {
            $this->errorno = self::ERR_INPUT_PARAM;
            $this->error = self::MSG_TYPE_NOT_SUPPORTED;
            return $this->errorno;
        }
        $status = $this->db->apply($this->key, 'lset', 'add', array($this->bin, $value));
        $this->processStatusCode($status);
        return $this->errorno;
    }

    /**
     * Adds values of a supported type to the LSet.
     * The elements added must be consistently the same type
     * (string, integer, array).
     *
     * @param array $values
     * @return int status code of the operation
     * @deprecated
     */
    public function addMany(array $values) {
        $status = $this->db->apply($this->key, 'lset', 'add_all', array($this->bin, $values));
        $this->processStatusCode($status);
        return $this->errorno;
    }

    /**
     * Check whether the element exists in the LSet.
     * Atomic elements (integer, string) will be directly compared. In complex
     * types (array) the value of a key named 'key' is used for comparison.
     *
     * @param int|string $value
     * @param boolean $found filled by the result of the operation
     * @return int status code of the operation
     * @deprecated
     */
    public function exists($value, &$found) {
        if (!is_string($value) && !is_int($value)) {
            $this->errorno = self::ERR_INPUT_PARAM;
            $this->error = self::MSG_TYPE_NOT_ATOMIC;
            $found = false;
            return $this->errorno;
        }
        $elements = array();
        $status = $this->db->apply($this->key, 'lset', 'exists', array($this->bin, $value), $found);
        $this->processStatusCode($status);
        if ($status !== Aerospike::OK) {
            $found = false;
        } else {
            $found = (boolean) $found;
        }
        return $this->errorno;
    }

    /**
     * Find and remove the element matching the given value from the LSet.
     * Atomic elements (integer, string) will be directly compared. In complex
     * types (array) the value of a key named 'key' is used to identify the
     * element which is to be removed.
     *
     * @param int|string $value
     * @return int status code of the operation
     * @deprecated
     */
    public function remove($value) {
        if (!is_string($value) && !is_int($value)) {
            $this->errorno = self::ERR_INPUT_PARAM;
            $this->error = self::MSG_TYPE_NOT_ATOMIC;
            return $this->errorno;
        }
        $status = $this->db->apply($this->key, 'lset', 'remove', array($this->bin, $value));
        $this->processStatusCode($status);
        return $this->errorno;
    }

    /**
     * Returns the elements in the LSet.
     *
     * @param array $elements returned
     * @return int status code of the operation
     * @deprecated
     */
    public function scan(&$elements) {
        $elements = array();
        $status = $this->db->apply($this->key, 'lset', 'scan', array($this->bin), $elements);
        $this->processStatusCode($status);
        return $status;
    }

    /**
     * Scan the LSet and apply a UDF to its elements.
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
