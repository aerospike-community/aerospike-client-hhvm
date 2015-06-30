--TEST--
Scan - Check for all positive arguments

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("Scan", "testScanPositiveWithAllArguments");
--XFAIL--
Failed because OPT_SCAN_CONCURRENTLY option is true and is not yet supported
--EXPECT--
OK

