--TEST--
ScanApply - Unknown namespace and set

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("ScanApply", "testScanApplyEmptyNamespaceSetNegative");
--XFAIL--
Failed because UDF AIPs are getting used here and are yet not implemented
--EXPECT--
ERR_PARAM

