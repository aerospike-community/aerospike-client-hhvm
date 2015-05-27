--TEST--
PUT Nested List of objects with PHP Serializer.  

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("Get", "testGetNestedListOfObjectsWithPHPSerializer");
--XFAIL--
Fails because serializer support is not present.
--EXPECT--
OK
