
# The Aerospike Large Stack class (deprecated)

Large Stack (lstack) is a large data type optimized for stack-based operations,
such as push and peek. The lstack provides the ability to continously grow a
very large collection of data.

```php

\Aerospike\LDT\LStack extends \Aerospike\LDT
{
    public __construct ( Aerospike $db, array $key, string $bin )
    public int push ( int|string|array $value )
    public int pushMany ( array $values )
    public int peek ( int $num, array &$elements )
    public int remove ( int|string $key )
    public int scan ( array &$elements )

    // To be implemented:
    // public int filter ( string $module, string $function, array $args, array &$elements )
    // public int pop ( int|string|array &$element )

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

