--TEST--
ScanApply - Scan Id is null string

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("ScanApply", "testScanApplyScanIdIsNull");
--XFAIL--
Failed because UDF AIPs are getting used here and are yet not implemented
--EXPECT--
OK
