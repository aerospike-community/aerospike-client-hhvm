--TEST--
ScanApply - Percent is string

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("ScanApply", "testScanApplyPercentIsString");
--XFAIL--
Failed because UDF AIPs are getting used here and are yet not implemented
--EXPECT--
ERR_CLIENT
