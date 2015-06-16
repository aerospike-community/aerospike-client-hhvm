--TEST--
ScanApply - Less arguments in udf function definition

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("ScanApply", "testScanApplyLuaLessParameter");
--XFAIL--
Failed because UDF AIPs are getting used here and are yet not implemented
--EXPECT--
OK
