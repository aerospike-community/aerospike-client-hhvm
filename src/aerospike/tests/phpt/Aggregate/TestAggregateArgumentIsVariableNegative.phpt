--TEST--
Aggregate - argument is variable

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("Aggregate", "testAggregateArgumentIsVariableNegative");
--EXPECT--
ERR_PARAM
