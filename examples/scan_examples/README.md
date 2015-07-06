# Scan Method Examples

### Simple Scan
`standard.php` shows how a callback method is invoked for each
record streamed from the server as it is scans the _users_ set of the _test_
namespace.

```bash
hhvm standard.php --host=192.168.119.3 -a -c
```

### Buffering Results With a Limit
`buffered.php` shows how records streaming back from a scan are
buffered into a _$result_ array, with the scan halted by returning `false` once
a predetermined limit is reached.

```bash
hhvm buffered.php --host=192.168.119.3 -a -c
```

