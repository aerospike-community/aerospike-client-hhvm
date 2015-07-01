# Scan Method Examples

There is a known HHVM bug ([issue 5527](https://github.com/facebook/hhvm/issues/5527)
which causes the scan examples to segfault when the HHVM JIT is on. These
examples **will** work if you run hhvm with the JIT turned off.

### Simple Scan
`standard.php` shows how a callback method is invoked for each
record streamed from the server as it is scans the _users_ set of the _test_
namespace.

```bash
hhvm  -v Eval.Jit=0 standard.php --host=192.168.119.3 -a -c
```

### Buffering Results With a Limit
`buffered.php` shows how records streaming back from a scan are
buffered into a _$result_ array, with the scan halted by returning `false` once
a predetermined limit is reached.

```bash
hhvm  -v Eval.Jit=0 buffered.php --host=192.168.119.3 -a -c
```

### Scan Applying a UDF in the Background
`background.php` shows how a UDF is applied to each record of a background scan
of a set _users_ set in namespace _test_, transforming the value in the _age_ bin
of those records.

After checking the scan info to see whether it has completed, the transformed
records are displayed.

```bash
hhvm  -v Eval.Jit=0 background.php --host=192.168.119.3 -a -c
```
