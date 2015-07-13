--TEST--
Aggregate - where parameter contains bool value

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("Aggregate", "testAggregateWithWhereContainingBoolValue");
--EXPECT--
ERR_CLIENT
