
# The Aerospike Large Ordered List class

Large Ordered List (llist) is optimized for searching and updating sorted
lists. It can access data at any point in the collection, while still being
capable of growing the collection to virtually any size.

```php

\Aerospike\LDT\LList extends \Aerospike\LDT
{
    public __construct ( Aerospike $db, array $key, string $bin )
    public int add ( int|string|array $value )
    public int addMany ( array $values )
    public int update ( int|string|array $value )
    public int updateMany ( array $values )
    public int exists ( int|string $value, boolean &$res )
    public int find ( int|string $value, array &$elements )
    public int findFirst ( int $count, array &$elements )
    public int findLast ( int $count, array &$elements )
    public int findRange ( int|string|null $min, int|string|null $max, array &$elements )
    public int scan ( array &$elements [, string $module=null [, string $function=null [, array $args=array()]]] )
    public int remove ( int|string $value )
    public int removeRange ( int|string|null $min, int|string|null $max )
    public int removeMany ( array $values )

    /* Inherited Methods */
    public boolean isLDT ( void )
    public boolean isValid ( void )
    public string error ( void )
    public int errorno ( void )
    public int size ( int &$num_elements )
    public int config ( array &$config )
    public int destroy ( void )
}
```

### [Aerospike Class](aerospike.md)
### [Aerospike LDT Abstract Class](aerospike_ldt.md)

