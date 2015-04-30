--TEST--
Get Map containig List of bools with PHP serializer.  

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("Get", "testGetMapListBoolsWithPHPSerializer");
--XFAIL--
Fails because serializer support is not present.
--EXPECT--
OK
