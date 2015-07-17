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
 * @link       http://www.aerospike.com/docs/guide/ldt.html
 * @filesource
 */

namespace Aerospike\LDT;
use Aerospike;

/**
 * Large Data Types (LDTs) allow individual record bins to contain collections
 * of hundreds of thousands of objects. Developers can employ LDTs to
 * manipulate large amounts of data quickly and efficiently, without being 
 * concerned with record or record bin size limitations.
 *
 * @package    Aerospike
 * @subpackage LDT
 * @link       http://www.aerospike.com/docs/guide/ldt.html
 * @author     Ronen Botzer <rbotzer@aerospike.com>
 */
abstract class LDT
{
    // LDT Types:
    const LLIST  = 1; // Large Ordered List
    const LMAP   = 2; // Large Map
    const LSET   = 3; // Large Set
    const LSTACK = 4; // Large Stack

    /* LDT Status Codes:
     * Each Aerospike API method invocation returns a status code
     *  depending upon the success or failure condition of the call.
     *
     * The error status codes map to the C client AEROSPIKE_ERR_LDT_* codes
     *  src/include/aerospike/as_status.h
     */
    const OK                        =    0;
    /**
     * Generic LDT Error
     */
    const ERR_LDT                   = 1300;
    /**
     * Generic input parameter error
     */
    const ERR_INPUT_PARAM           = 1409;
    /**
     * Generic server-side error
     */
    const ERR_INTERNAL              = 1400;
    /**
     * Element not found
     */
    const ERR_NOT_FOUND             = 1401;
    /**
     * Duplicate element written when 'unique key' set
     */
    const ERR_UNIQUE_KEY            = 1402;
    /**
     * Generic error for insertion op
     */
    const ERR_INSERT                = 1403;
    /**
     *  Generic error for search op
     */
    const ERR_SEARCH                = 1404;
    /**
     * Generic error for delete op
     */
    const ERR_DELETE                = 1405;
    /**
     * General input parameter error
     */
    const ERR_LDT_INPUT_PARM        = 1409;
    /**
     * LDT type mismatched for the bin
     */
    const ERR_TYPE_MISMATCH         = 1410;
    /**
     * The LDT bin name is null
     */
    const ERR_NULL_BIN_NAME         = 1411;
    /**
     * The LDT bin name must be a string
     */
    const ERR_BIN_NAME_NOT_STRING   = 1412;
    /**
     * The LDT bin name exceeds 14 chars
     */
    const ERR_BIN_NAME_TOO_LONG     = 1413;
    /**
     * Server-side error: open subrecs
     */
    const ERR_TOO_MANY_OPEN_SUBRECS = 1414;
    /**
     * record containing the LDT not found
     */
    const ERR_TOP_REC_NOT_FOUND     = 1415;
    /**
     * Server-side error: subrec not found
     */
    const ERR_SUB_REC_NOT_FOUND     = 1416;
    /**
     * LDT bin does not exist
     */
    const ERR_BIN_DOES_NOT_EXIST    = 1417;
    /**
     * Collision creating LDT at bin
     */
    const ERR_BIN_ALREADY_EXISTS    = 1418;
    /**
     * Control structures in the top record are damaged
     */
    const ERR_BIN_DAMAGED           = 1419;
    /**
     * Subrec pool is damaged
     */
    const ERR_SUBREC_POOL_DAMAGED   = 1420;
    /**
     * Control structures in the sub record are damaged
     */
    const ERR_SUBREC_DAMAGED        = 1421;
    /**
     * Error while opening the sub record
     */
    const ERR_SUBREC_OPEN           = 1422;
    /**
     * Error while updating the sub record
     */
    const ERR_SUBREC_UPDATE         = 1423;
    /**
     * Error while creating the sub record
     */
    const ERR_SUBREC_CREATE         = 1424;
    /**
     * Error while deleting the sub record
     */
    const ERR_SUBREC_DELETE         = 1425;
    /**
     * Error while closing the sub record
     */
    const ERR_SUBREC_CLOSE          = 1426;
    /**
     * Error while updating the top record
     */
    const ERR_TOPREC_UPDATE         = 1427;
    /**
     * Error while updating the top record
     */
    const ERR_TOPREC_CREATE         = 1428;
    /**
     * The filter function name was invalid
     */
    const ERR_LDT_FILTER_FUNCTION_BAD       = 1430;
    /**
     * The filter function could not be found
     */
    const ERR_LDT_FILTER_FUNCTION_NOT_FOUND = 1431;
    /**
     * Invalid function used to extract a unique value from a complex object
     */
    const ERR_LDT_KEY_FUNCTION_BAD          = 1432;
    /**
     * Could not find the 'key function' or the input given
     * to the default key function is invalid. For complex types the form
     * array('key'=> _value_) is expected.
     */
    const ERR_LDT_KEY_FUNCTION_NOT_FOUND    = 1433;

    /**
     * The key for the record containing the LDT
     * @var string
     */
    protected $key;
    /**
     * The name of the bin within the recod that is an LDT
     * @var string
     */
    protected $bin;
    /**
     * The type of LDT at the bin
     * @var string
     */
    protected $type;
    /**
     * The name of the UDF module providing the functionality that this class wraps around
     * @var string
     */
    protected $module;
    /**
     * The database connection class
     * @var Aerospike
     */
    protected $db;
    /**
     * The error message for the last operation
     * @var string
     * @see LDT::error()
     */
    protected $error;
    /**
     * The status code for the last operation
     * @var integer
     * @see LDT::errorno()
     */
    protected $errorno;

    /**
     * Constructor for the abstract \Aerospike\LDT class. Inherited by LDT types.
     *
     * @param Aerospike $db
     * @param array $key initialized with Aerospike::initKey()
     * @param string $bin name
     * @param int $type of the LDT
     * @see errorno()
     * @see error()
     */
    protected function __construct(Aerospike $db, array $key, $bin, $type) {
        $this->key = $key;
        $this->bin = $bin;
        $this->type = $type;
        $this->module = $this->getModuleName();
        $this->db = $db;
        if (!$db->isConnected()) {
            if ($db->errorno() !== Aerospike::OK) {
                $this->errorno = $db->errorno();
                $this->error = $db->error();
            } else {
                $this->errorno = Aerospike::ERR_CLUSTER;
                $this->error = "Aerospike object could not successfully connect to the cluster";
            }
        } else {
            $this->errorno = Aerospike::OK;
            $this->error = '';
        }
    }

    /**
     * Returns the error message for the previous operation.
     *
     * @return string
     */
    public function error() {
        return $this->error;
    }

    /**
     * Returns the status code for the previous operation.
     *
     * @return int
     */
    public function errorno() {
        return $this->errorno;
    }

    /**
     * Checks whether there actually is an LDT at the key and bin the class
     * was initialized with.
     *
     * @return boolean
     */
    public function isLDT() {
        $status = $this->db->apply($this->key, $this->module, 'ldt_exists', array($this->bin), $returned);
        $this->processStatusCode($status);
        if ($status !== Aerospike::OK) {
            return false;
        } else {
            return (boolean) $returned;
        }
    }

    /**
     * Validates whether the LDT at the specified key and bin is in good shape
     *
     * @return boolean
     */
    public function isValid() {
        $status = $this->db->apply($this->key, $this->module, 'ldt_validate', array($this->bin), $returned);
        $this->processStatusCode($status);
        if ($status !== Aerospike::OK) {
            return false;
        } else {
            return (boolean) $returned;
        }
    }

    /**
     * Sets $num_elements with the number of elements in the LDT.
     *
     * @param int $num_elements returned
     * @return int status code of the operation
     */
    public function size(&$num_elements) {
        $status = $this->db->apply($this->key, $this->module, 'size', array($this->bin), $num_elements);
        $this->processStatusCode($status);
        return $status;
    }

    /**
     * Retrieves the LDT configuration data.
     *
     * @param array $config will be updated with the configuration data
     * @return int status code of the operation
     */
    public function config(&$config) {
        $config = array();
        $status = $this->db->apply($this->key, 'llist', 'config', array($this->bin), $config);
        $this->processStatusCode($status);
        return $this->errorno;
    }

    /**
     * Set the page size of the LDT
     *
     * @param int $size
     * @return int status code of the operation
     */
    public function setPageSize($size) {
        $size = intval($size);
        if ($size <= 0) {
            $this->errorno = self::ERR_INPUT_PARAM;
            $this->error = self::MSG_TYPE_NOT_SUPPORTED;
            return $this->errorno;
        }
        $status = $this->db->apply($this->key, $this->module, 'setPageSize', array($this->bin, $size));
        $this->processStatusCode($status);
        return $status;
    }

    /**
     * Destroy the LDT at the key and bin the class was initialized to.
     *
     * @return int status code of the operation
     */
    public function destroy() {
        $status = $this->db->apply($this->key, $this->module, 'destroy', array($this->bin));
        $this->processStatusCode($status);
        return $status;
    }

    /**
     * @param int $num_elements returned
     * @return int status code of the operation
     * @deprecated
     */
    public function getCapacity(&$num_elements) {
        return LDT::OK;
    }

    /**
     * Sets the max number of elements that the LDT can hold.
     *
     * @param int $num_elements
     * @return int status code of the operation
     * @deprecated
     */
    public function setCapacity($num_elements) {
        return LDT::OK;
    }

    /**
     * Process the operation status code into error number and message
     *
     * @param int $status code of the operation
     */
    protected function processStatusCode($status) {
        if ($status === Aerospike::OK) {
            $this->error = '';
            $this->errorno = self::OK;
            return true;
        }
        if ($status === Aerospike::ERR_UDF) {
            // parse the secondary error code embedded in the error message
            $rhs = strrpos($this->db->error(), ':LDT');
            $lhs = strrpos($this->db->error(), ': ');
            if ($rhs !== false && $lhs !== false) {
                $this->errorno = (int) substr($this->db->error(), $lhs + 2, ($rhs - ($lhs + 2)));
                $this->error = substr($this->db->error(), $rhs + 1);
                return true;
            }
        }
        $this->error = $this->db->error();
        $this->errorno = $this->db->errorno();
    }

    /**
     * Finds the name of the UDF module providing the functionality needed by the given LDT
     * @return string
     */
    private function getModuleName() {
        switch($this->type) {
            case self::LLIST:
                return 'llist';
            case self::LMAP:
                return 'lmap';
            case self::LSET:
                return 'lset';
            case self::LSTACK:
                return 'lstack';
            default:
                return '';
        }
    }

}

?>
