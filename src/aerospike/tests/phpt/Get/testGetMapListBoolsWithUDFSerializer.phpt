--TEST--
Get Map containig List of bools with UDF serializer. 

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("Get", "testGetMapListBoolsWithUDFSerializer");
--XFAIL--
Fails because serializer support is not present.
--EXPECT--
OK
