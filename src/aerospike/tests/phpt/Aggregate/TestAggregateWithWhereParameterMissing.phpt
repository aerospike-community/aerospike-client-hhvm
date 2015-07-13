--TEST--
Aggregate - where parameter missing

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("Aggregate", "testAggregateWithWhereParameterMissing");
--EXPECT--
ERR_PARAM

