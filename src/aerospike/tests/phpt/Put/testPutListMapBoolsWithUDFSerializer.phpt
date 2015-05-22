--TEST--
PUT List containing Map of bools with UDF serializer

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("Put", "testPutListMapBoolsWithUDFSerializer");
--XFAIL--
Fails because serialization and deserialization support is not present.
--EXPECT--
OK
