--TEST--
Get Nested List of bools with PHP serializer.  

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("Get", "testGetNestedListOfBoolsWithPHPSerializer");
--XFAIL--
Fails because serializer support is not present.
--EXPECT--
OK
