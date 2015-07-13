--TEST--
Aggregate - correct arguments with policy

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("Aggregate", "testAggregatePositiveWithPolicy");
--EXPECT--
OK
