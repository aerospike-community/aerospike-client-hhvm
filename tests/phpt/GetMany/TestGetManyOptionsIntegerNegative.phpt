--TEST--
GetMany - with option as integer

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("GetMany", "testGetManyOptionsIntegerNegative");
--EXPECTREGEX--
OK|ERR_PARAM

