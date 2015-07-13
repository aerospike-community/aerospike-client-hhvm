--TEST--
Aggregate - where parameter contains array value

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("Aggregate", "testAggregateWithWhereContainingArrayValue");
--EXPECT--
OK

