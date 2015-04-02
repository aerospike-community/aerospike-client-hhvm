# Performance Scripts

## Single Process
The read, write, and read-write-mix scripts do basic clocking of single process
performance, one client talking to one cluster.

### Write Performance
`write.php` writes n times to a record with key ("test", "performance", "write")

```bash
hhvm -d extension_dir=../ -d hhvm.extensions[]=aerospike-hhvm.so write.php --host=192.168.119.3 --num-ops=100000
```

### Read Performance
`read.php` reads n times from a record with key ("test", "performance", "Write")

```bash
hhvm -d extension_dir=../ -d hhvm.extensions[]=aerospike-hhvm.so read.php --host=192.168.119.3 --num-ops=100000
```

### Read-Write Performance
`read-write-mix.php` measures combined writes and reads with a given ratio.

For example a test of 250k read-write operations with a 9:1 ratio:

```bash
hhvm -d extension_dir=../ -d hhvm.extensions[]=aerospike-hhvm.so read-write-mix.php --host=192.168.119.3 --num-ops=250000 --write-every=10
```

