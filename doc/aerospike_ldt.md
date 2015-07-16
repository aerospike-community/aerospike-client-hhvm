
# The Aerospike Large Data Type classes
Large Data Types (LDTs) allow individual record bins to contain collections of
hundreds of thousands of objects.  Developers can employ LDTs to manipulate
large amounts of data quickly and efficiently, without being concerned with
record or record bin size limitations.

Common functionality is provided by the abstract class **AerospikeLDT**.
For each LDT type there is a matching concrete subclass:

* [\Aerospike\LDT\LList](aerospike_llist.md) - The Aerospike [Large Ordered List](http://www.aerospike.com/docs/guide/llist.html) class

The following LDT types have been [deprecated](https://discuss.aerospike.com/t/aerospike-server-ce-3-5-2-february-13-2015/975),
with their functionality rolled into LList.
* [\Aerospike\LDT\LSet](aerospike_lset.md) - The Aerospike [Large Set](http://www.aerospike.com/docs/guide/lset.html) class
* [\Aerospike\LDT\LMap](aerospike_lmap.md) - The Aerospike [Large Map](http://www.aerospike.com/docs/guide/lmap.html) class
* [\Aerospike\LDT\LStack](aerospike_lstack.md) - The Aerospike [Large Stack](http://www.aerospike.com/docs/guide/lstack.html) class

Full documentation of LDTs is available at the [Aerospike website](http://www.aerospike.com/docs/guide/ldt.html).

## Abstract Class \Aerospike\LDT

```php

abstract \Aerospike\LDT
{
    // LDT Types:
    //  maps to the C client src/include/aerospike/as_ldt.h
    const int LLIST = 1;
    const int LMAP = 2;
    const int LSET = 3;
    const int LSTACK = 4;

    /* LDT Status Codes:
     * Each Aerospike API method invocation returns a status code
     *  depending upon the success or failure condition of the call.
     *
     * The error status codes map to the C client AEROSPIKE_ERR_LDT_* codes
     *  src/include/aerospike/as_status.h
     */
    const OK                        =    0; // Success
    const ERR_INPUT_PARAM           = 1409; // Generic input parameter error
    const ERR_INTERNAL              = 1400; // Generic server-side error
    const ERR_NOT_FOUND             = 1401; // Element not found
    const ERR_UNIQUE_KEY            = 1402; // Duplicate element written when 'unique key' set
    const ERR_INSERT                = 1403; // Generic error for insertion op
    const ERR_SEARCH                = 1404; // Generic error for search op
    const ERR_DELETE                = 1405; // Generic error for delete op
    const ERR_TYPE_MISMATCH         = 1410; // LDT type mismatched for the bin
    const ERR_NULL_BIN_NAME         = 1411; // The LDT bin name is null
    const ERR_BIN_NAME_NOT_STRING   = 1412; // The LDT bin name must be a string
    const ERR_BIN_NAME_TOO_LONG     = 1413; // The LDT bin name exceeds 14 chars
    const ERR_TOO_MANY_OPEN_SUBRECS = 1414; // Server-side error: open subrecs
    const ERR_TOP_REC_NOT_FOUND     = 1415; // record containing the LDT not found
    const ERR_SUB_REC_NOT_FOUND     = 1416; // Server-side error: subrec not found
    const ERR_BIN_DOES_NOT_EXIST    = 1417; // LDT bin does not exist
    const ERR_BIN_ALREADY_EXISTS    = 1418; // Collision creating LDT at bin
    const ERR_BIN_DAMAGED           = 1419; // Control structures in the top record are damaged
    const ERR_SUBREC_POOL_DAMAGED   = 1420; // Subrec pool is damaged
    const ERR_SUBREC_DAMAGED        = 1421; // Control structures in the sub record are damaged
    const ERR_SUBREC_OPEN           = 1422; // Error while opening the sub record
    const ERR_SUBREC_UPDATE         = 1423; // Error while updating the sub record
    const ERR_SUBREC_CREATE         = 1424; // Error while creating the sub record
    const ERR_SUBREC_DELETE         = 1425; // Error while deleting the sub record
    const ERR_SUBREC_CLOSE          = 1426; // Error while closing the sub record
    const ERR_TOPREC_UPDATE         = 1427; // Error while updating the top record
    const ERR_TOPREC_CREATE         = 1428; // Error while creating the top record


    protected __construct ( Aerospike $db, array $key, string $bin, int $type )
    public boolean isLDT ( void )
    public boolean isValid ( void )
    public string error ( void )
    public int errorno ( void )
    public int size ( int &$num_elements )
    public int config ( array &$config )
    public void setPageSize ( int $size )
    public int destroy ( void )
}
```

### [Aerospike Class](aerospike.md)

