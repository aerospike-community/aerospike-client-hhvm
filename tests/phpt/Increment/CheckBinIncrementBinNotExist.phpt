--TEST--
Increment - bin value by offset bin not exist

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("Increment", "testBinIncrementBinNotExist");
--XFAIL--
Fails because get() with filter bins support is not present, Which is used in the testcase.
--EXPECT--
OK
