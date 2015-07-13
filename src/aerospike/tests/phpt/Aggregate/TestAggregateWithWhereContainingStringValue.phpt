--TEST--
Aggregate - where array containing string values

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("Aggregate", "testAggregateWithWhereContainingStringValue");
--EXPECT--
OK

