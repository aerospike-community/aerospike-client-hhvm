--TEST--
Get Nested Map of floats with PHP serializer.  

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("Get", "testGetNestedMapOfFloatsWithPHPSerializer");
--XFAIL--
Fails because serializer support is not present.
--EXPECT--
OK
