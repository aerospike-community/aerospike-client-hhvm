--TEST--
PUT Nested List of bools with PHP serializer.  

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("Put", "testPutNestedListOfObjectsWithPHPSerializer");
--XFAIL--
Fails because serialization and deserialization support is not present.
--EXPECT--
OK
