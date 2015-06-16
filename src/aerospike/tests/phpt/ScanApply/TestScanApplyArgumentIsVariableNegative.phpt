--TEST--
ScanApply - Argument is variable

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("ScanApply", "testScanApplyArgumentIsVariableNegative");
--XFAIL--
Failed because UDF AIPs are getting used here and are yet not implemented
--EXPECT--
ERR_PARAM

