--TEST--
PUT Nested List of objects with UDF Serializer.  

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("Put", "testPutNestedListOfObjectsWithUDFSerializer");
--XFAIL--
Fails because serialization and deserialization support is not present.
--EXPECT--
OK
