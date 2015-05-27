--TEST--
Put NULL data within mixed nested data.

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("Put", "testPutNullDataMixedPositive");
--XFAIL--
Fails because serialization and deserialization support is not present.
--EXPECT--
OK
