--TEST--
PUT Nested Map of floats with UDF serializer.  

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("Put", "testPutNestedMapOfFloatsWithUDFSerializer");
--XFAIL--
Fails because serialization and deserialization support is not present.
--EXPECT--
OK
