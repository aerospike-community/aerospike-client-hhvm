
# The Aerospike Large Set class (deprecated)

Large Set (lset) is a large data type optimized for storing unique values
and checking the existance of a value in the set. Large sets are suited for
storing a large number of unique values.

```php

\Aerospike\LDT\LSet extends \Aerospike\LDT
{
    public __construct ( Aerospike $db, array $key, string $bin )
    public int add ( int|string|array $value )
    public int addMany ( array $values )
    public int exists ( int|string $value, boolean &$found )
    public int remove ( int|string $value )
    public int scan ( array &$elements )

    // To be implemented:
    // public int filter ( string $module, string $function, array $args, array &$elements )
    // public int get ( mixed $value, mixed &$element )

    /* Inherited Methods */
    public boolean isLDT ( void )
    public string error ( void )
    public int errorno ( void )
    public int size ( int &$num_elements )
    public int destroy ( void )
    public int getCapacity ( int &$num_elements )
    public int setCapacity ( int $num_elements )
}
```

### [Aerospike Class](aerospike.md)
### [Aerospike LDT Abstract Class](aerospike_ldt.md)

