
# The Aerospike Large Map class (deprecated)

Large Map (lmap) is a large data type that employs the standard "key/value"
map functionality. It is optimized for complex values where the key may be an
integer or string, and the value may be any supported data type.

```php

\Aerospike\LDT\LMap extends \Aerospike\LDT
{
    public __construct ( Aerospike $db, array $key, string $bin )
    public int put ( int|string $key, int|string|array $value )
    public int putMany ( array $key_vals )
    public int get ( int|string $key, int|string|array &$element )
    public int remove ( int|string $key )
    public int scan ( array &$elements )

    // To be implemented:
    // public int filter ( string $module, string $function, array $args, array &$elements )

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

