--TEST--
 PUT Nested map of objects with PHP Serializer.  

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("Put", "testPutNestedMapOfObjectsWithPHPSerializer");
--XFAIL--
Fails because serialization and deserialization support is not present.
--EXPECT--
OK
