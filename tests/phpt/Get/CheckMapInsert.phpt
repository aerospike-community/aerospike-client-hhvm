--TEST--
Get - Check Map insert in bin

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("Get", "testCheckMapInsert");
--XFAIL--
Fails due to filter bins support is not present for get().
--EXPECT--
OK
