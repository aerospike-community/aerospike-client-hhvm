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
 * @link       http://www.aerospike.com/docs/guide/llist.html
 * @filesource
 */
namespace Aerospike\LDT;
use Aerospike;

/**
 * Large Ordered List (llist) is optimized for searching and updating sorted
 * lists. It can access data at any point in the collection, while still being
 * capable of growing the collection to virtually any size.
 *
 * @package    Aerospike
 * @subpackage LDT
 * @link       http://www.aerospike.com/docs/guide/llist.html
 * @author     Ronen Botzer <rbotzer@aerospike.com>
 */
class LList extends LDT
{
    // error messages
    const MSG_TYPE_NOT_SUPPORTED = "\$value must be a supported type (string|integer|array)";
    const MSG_TYPE_NOT_ATOMIC = "\$value must be an atomic type (string|integer)";
    const MSG_RANGE_TYPE_INVALID = "\$min and \$max must be of type (string|integer|null)";

    /**
     * Constructor for the \Aerospike\LDT\LList class.
     *
     * @param Aerospike $db
     * @param array $key initialized with Aerospike::initKey()
     * @param string $bin name
     * @see LDT::__construct()
     */
    public function __construct(Aerospike $db, array $key, $bin) {
        parent::__construct($db, $key, $bin, LDT::LLIST);
    }

    /**
     * Adds a value of a supported type to the LList.
     * The elements added must be consistently the same type
     * (string, integer, array).
     *
     * @param int|string|array $value
     * @return int status code of the operation
     */
    public function add($value) {
        if (!is_string($value) && !is_int($value) && !is_array($value)) {
            $this->errorno = self::ERR_INPUT_PARAM;
            $this->error = self::MSG_TYPE_NOT_SUPPORTED;
            return $this->errorno;
        }
        $status = $this->db->apply($this->key, 'llist', 'add', array($this->bin, $value));
        $this->processStatusCode($status);
        return $this->errorno;
    }

    /**
     * Adds values of a supported type to the LList.
     * The elements added must be consistently the same type
     * (string, integer, array).
     *
     * @param array $values
     * @return int status code of the operation
     */
    public function addMany(array $values) {
        $status = $this->db->apply($this->key, 'llist', 'add_all', array($this->bin, $values));
        $this->processStatusCode($status);
        return $this->errorno;
    }

    /**
     * Inserts a new element into the LList or replaces the matching one.
     * The elements must be consistently the same type
     * (string, integer, array).
     *
     * @param int|string|array $value
     * @return int status code of the operation
     */
    public function update($value) {
        if (!is_string($value) && !is_int($value) && !is_array($value)) {
            $this->errorno = self::ERR_INPUT_PARAM;
            $this->error = self::MSG_TYPE_NOT_SUPPORTED;
            return $this->errorno;
        }
        $status = $this->db->apply($this->key, 'llist', 'update', array($this->bin, $value));
        $this->processStatusCode($status);
        return $this->errorno;
    }

    /**
     * Updates several elements in the LList, the plural of update().
     * The elements updated must be consistently the same type
     * (string, integer, array).
     *
     * @param array $values
     * @return int status code of the operation
     */
    public function updateMany(array $values) {
        $status = $this->db->apply($this->key, 'llist', 'update_all', array($this->bin, $values));
        $this->processStatusCode($status);
        return $this->errorno;
    }

    /**
     * Finds whether any elements match the given value in the LList.
     * Atomic elements (integer, string) will be directly compared. In complex
     * types (array) the value of a key named 'key' is used for comparison.
     *
     * @param int|string $value
     * @param boolean $res
     * @return int status code of the operation
     */
    public function exists($value, &$res) {
        if (!is_string($value) && !is_int($value) && !is_array($value)) {
            $this->errorno = self::ERR_INPUT_PARAM;
            $this->error = self::MSG_TYPE_NOT_SUPPORTED;
            return $this->errorno;
        }
        $status = $this->db->apply($this->key, 'llist', 'exists', array($this->bin, $value), $res);
        $res = (bool) $res;
        $this->processStatusCode($status);
        return $this->errorno;
    }

    /**
     * Finds the elements matching the given value in the LList.
     * Atomic elements (integer, string) will be directly compared. In complex
     * types (array) the value of a key named 'key' is used for comparison.
     *
     * @param int|string $value
     * @param array $elements matched
     * @return int status code of the operation
     */
    public function find($value, &$elements) {
        if (!is_string($value) && !is_int($value) && !is_array($value)) {
            $this->errorno = self::ERR_INPUT_PARAM;
            $this->error = self::MSG_TYPE_NOT_SUPPORTED;
            return $this->errorno;
        }
        $elements = array();
        $status = $this->db->apply($this->key, 'llist', 'find', array($this->bin, $value), $elements);
        $this->processStatusCode($status);
        return $this->errorno;
    }

    /**
     * @deprecated use findFirst instead
     */
    public function find_first($count, &$elements) {
        return $this->findFirst($count, $elements);
    }

    /**
     * Finds the first N elements in the LList.
     *
     * @param int $count
     * @param array $elements matched
     * @return int status code of the operation
     */
    public function findFirst($count, &$elements) {
        if (!is_int($count)) {
            $this->errorno = self::ERR_INPUT_PARAM;
            $this->error = self::MSG_TYPE_NOT_SUPPORTED;
            return $this->errorno;
        }
        $elements = array();
        $status = $this->db->apply($this->key, 'llist', 'find_first', array($this->bin, $count), $elements);
        $this->processStatusCode($status);
        return $this->errorno;
    }

    /**
     * @deprecated use findLast instead
     */
    public function find_last($count, &$elements) {
        return $this->findLast($count, $elements);
    }

    /**
     * Finds the lasst N elements in the LList.
     *
     * @param int $count
     * @param array $elements matched
     * @return int status code of the operation
     */
    public function findLast($count, &$elements) {
        if (!is_int($count)) {
            $this->errorno = self::ERR_INPUT_PARAM;
            $this->error = self::MSG_TYPE_NOT_SUPPORTED;
            return $this->errorno;
        }
        $elements = array();
        $status = $this->db->apply($this->key, 'llist', 'find_last', array($this->bin, $count), $elements);
        $this->processStatusCode($status);
        return $this->errorno;
    }

    /**
     * Finds the elements whose key is between min and max.
     *
     * A null $min gets all elements less than or equal to $max.
     * A null $max gets all elements greater than or equal to $min.
     *
     * @param int|string|null $min
     * @param int|string|null $max
     * @param array $elements matched
     * @return int status code of the operation
     */
    public function findRange($min, $max, &$elements) {
        if (!is_int($min) && !is_null($min) && !is_int($max) && !is_null($max)) {
            $this->errorno = self::ERR_INPUT_PARAM;
            $this->error = self::MSG_TYPE_NOT_SUPPORTED;
            return $this->errorno;
        }
        $elements = array();
        $status = $this->db->apply($this->key, 'llist', 'find_range', array($this->bin, $min, $max), $elements);
        $this->processStatusCode($status);
        return $this->errorno;
    }

    /**
     * Returns the elements in the LList
     *
     * An optional UDF filter function can be applied to the elements found.
     * The filter function returns nil to filter out the element, otherwise it
     * may transform the element before returning it.
     *
     * @param array $elements returned
     * @param string|null $module the name of the UDF module containing the optional filter function
     * @param string|null $function name of the UDF filter function to apply
     * @param array $args optional arguments for the filter function
     * @return int status code of the operation
     */
    public function scan(&$elements, $module=null, $function=null, array $args=array()) {
        $elements = array();
        if (!is_null($module) && !is_null($function)) {
            $status = $this->db->apply($this->key, 'llist', 'filter', array($this->bin, $module, $function, $args), $elements);
        } else {
            $status = $this->db->apply($this->key, 'llist', 'scan', array($this->bin), $elements);
        }
        $this->processStatusCode($status);
        return $this->errorno;
    }

    /**
     * Find and remove an element matching the given value in the LList.
     * Atomic elements (integer, string) will be directly compared. In complex
     * types (array) the value of a key named 'key' is used to identify the
     * element which is to be removed.
     *
     * @param int|string $value
     * @return int status code of the operation
     */
    public function remove($value) {
        if (!is_string($value) && !is_int($value)) {
            $this->errorno = self::ERR_INPUT_PARAM;
            $this->error = self::MSG_TYPE_NOT_ATOMIC;
            return $this->errorno;
        }
        $status = $this->db->apply($this->key, 'llist', 'remove', array($this->bin, $value));
        $this->processStatusCode($status);
        return $this->errorno;
    }

    /**
     * Find and remove the elements from the LList matching a given range.
     * Atomic elements (integer, string) will be directly compared. In complex
     * types (array) the value of a key named 'key' is used to identify the
     * elements which are to be removed.
     *
     * A null $min gets all elements less than or equal to $max.
     * A null $max gets all elements greater than or equal to $min.
     *
     * @param int|string|null $min
     * @param int|string|null $max
     * @return int status code of the operation
     */
    public function removeRange($min=null, $max=null) {
        if ((!is_string($min) && !is_int($min) && !is_null($min)) ||
            (!is_string($max) && !is_int($max) && !is_null($max))) {
            $this->errorno = self::ERR_INPUT_PARAM;
            $this->error = self::MSG_RANGE_TYPE_INVALID;
            return $this->errorno;
        }
        $status = $this->db->apply($this->key, 'llist', 'remove_range', array($this->bin, $min, $max));
        $this->processStatusCode($status);
        return $this->errorno;
    }

    /**
     * Removes several elements in the LList, the plural of remove().
     * The elements removed must be consistently the same type
     * (string, integer, array).
     *
     * @param array $values
     * @return int status code of the operation
     */
    public function removeMany(array $values) {
        $status = $this->db->apply($this->key, 'llist', 'remove_all', array($this->bin, $values));
        $this->processStatusCode($status);
        return $this->errorno;
    }

}

?>
