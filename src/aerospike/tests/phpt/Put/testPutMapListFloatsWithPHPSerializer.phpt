--TEST--
PUT Map containing List of floats with PHP serializer.  

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("Put", "testPutMapListFloatsWithPHPSerializer");
--XFAIL--
Fails because serialization and deserialization support is not present.
--EXPECT--
OK
