--TEST--
Scaninfo - scan id is a string

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("ScanInfo", "testScanInfoScanIdString");
--XFAIL--
Failed because UDF AIPs are getting used here and are yet not implemented
--EXPECT--
ERR_PARAM

