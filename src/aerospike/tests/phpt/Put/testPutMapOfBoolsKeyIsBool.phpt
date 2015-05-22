--TEST--
Put - Boolean value asa a key

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("Put", "testPutMapOfBoolsKeyIsBool");
--XFAIL--
Fails because serialization and deserialization support is not present.
--EXPECT--
OK
