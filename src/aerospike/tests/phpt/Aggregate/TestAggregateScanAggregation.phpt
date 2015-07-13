--TEST--
Aggregate - Scan aggregation (empty array given as predicate)

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("Aggregate", "testScanAggregation");
--XFAIL--
Waiting on fix to aggregate()
--EXPECT--
OK

