--TEST--
 Get Map containing List of floats with UDF serializer.  

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("Get", "testGetMapListFloatsWithUDFSerializer");
--XFAIL--
Fails because serializer support is not present.
--EXPECT--
OK
