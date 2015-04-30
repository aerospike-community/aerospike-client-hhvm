--TEST--
Get Nested map of objects with UDF Serializer.  

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("Get", "testGetNestedMapOfObjectsWithUDFSerializer");
--XFAIL--
Fails because serializer support is not present.
--EXPECT--
OK
