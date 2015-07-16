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
 * @link       http://www.aerospike.com/docs/guide/lstack.html
 * @filesource
 * @deprecated
 */
namespace Aerospike\LDT;
use Aerospike;

/**
 * Large Stack (lstack) is a large data type optimized for stack-based
 * operations, such as push and peek. The lstack provides the ability to
 * continously grow a very large collection of data.
 *
 * @package    Aerospike
 * @subpackage LDT
 * @link       http://www.aerospike.com/docs/guide/lstack.html
 * @link       http://www.aerospike.com/docs/guide/data-types.html
 * @author     Ronen Botzer <rbotzer@aerospike.com>
 * @deprecated
 */
class LStack extends LDT
{
    // error messages
    const MSG_TYPE_NOT_SUPPORTED = "\$value must be a supported type (string|integer|array)";
    const MSG_NUM_NOT_INT = "\$num must be an integer";

    /**
     * Constructor for the \Aerospike\LDT\LStack class.
     *
     * @param Aerospike $db
     * @param array $key initialized with Aerospike::initKey()
     * @param string $bin name
     * @see LDT::__construct()
     * @deprecated
     */
    public function __construct(Aerospike $db, array $key, $bin) {
        parent::__construct($db, $key, $bin, LDT::LSTACK);
    }

    /**
     * Push the value pair onto the LStack.
     *
     * @param int|string|array $value
     * @return int status code of the operation
     * @deprecated
     */
    public function push($value) {
        if (!is_string($value) && !is_int($value) && !is_array($value)) {
            $this->errorno = self::ERR_INPUT_PARAM;
            $this->error = self::MSG_TYPE_NOT_SUPPORTED;
            return $this->errorno;
        }
        $status = $this->db->apply($this->key, 'lstack', 'push', array($this->bin, $value));
        $this->processStatusCode($status);
        return $this->errorno;
    }

    /**
     * Push multiple values onto the LStack.
     * The elements added must be consistently the same type
     * (string, integer, array).
     *
     * @param array $values
     * @return int status code of the operation
     * @see push
     * @deprecated
     */
    public function pushMany(array $values) {
        $status = $this->db->apply($this->key, 'lstack', 'push_all', array($this->bin, $values));
        $this->processStatusCode($status);
        return $this->errorno;
    }

    /**
     * Finds the element at the $num position in the LStack.
     *
     * @param int $num of elements to peek at from the top of stack.
     * @param array $elements matched
     * @return int status code of the operation
     * @deprecated
     */
    public function peek($num, &$elements) {
        if (!is_int($num)) {
            $this->errorno = self::ERR_INPUT_PARAM;
            $this->error = self::MSG_NUM_NOT_INT;
            return $this->errorno;
        }
        $elements = array();
        $status = $this->db->apply($this->key, 'lstack', 'peek', array($this->bin, $num), $elements);
        $this->processStatusCode($status);
        return $this->errorno;
    }

    /**
     * Remove an element from the top of the LStack.
     *
     * @todo To be implemented.
     * @param int|string|array $element
     * @return int status code of the operation
     * @deprecated
     */
    public function pop(&$element) {
        $this->error = "Method not implemented";
        $this->errorno = self::ERR_LDT;
        return $this->errorno;
    }

    /**
     * Returns the elements in the LStack.
     *
     * @todo To be implemented.
     * @param array $elements returned
     * @return int status code of the operation
     * @deprecated
     */
    public function scan(&$elements) {
        $this->error = "Method not implemented";
        $this->errorno = self::ERR_LDT;
        return $this->errorno;
    }

    /**
     * Scan the LStack and apply a UDF to its elements.
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
