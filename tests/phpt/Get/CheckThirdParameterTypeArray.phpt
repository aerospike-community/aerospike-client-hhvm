--TEST--
Get - GET With Third Parameter not an array.

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("Get", "testCheckThirdParameterTypeArray");
--XFAIL--
Fails due to filter bins support is not present for get().
--EXPECT--
ERR_PARAM
