--TEST--
Aggregate - Returns multiple records

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("Aggregate", "testAggregateReturnMultipleRecords");
--EXPECT--
OK

