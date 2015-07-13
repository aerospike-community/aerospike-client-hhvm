--TEST--
Aggregate - empty function udf name

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("Aggregate", "testAggregateEmptyFunctionUDFNameNegative");
--EXPECT--
ERR_PARAM
