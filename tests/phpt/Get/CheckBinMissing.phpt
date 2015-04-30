--TEST--
Get - bin missing in selected bin array

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("Get", "testCheckBinMissing");
--XFAIL--
Fails due to filter bins support is not present for get().
--EXPECT--
ERR_PARAM
