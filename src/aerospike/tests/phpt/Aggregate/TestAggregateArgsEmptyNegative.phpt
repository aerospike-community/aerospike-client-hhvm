--TEST--
Aggregate - empty args to udf

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("Aggregate", "testAggregateArgsEmptyNegative");
--EXPECT--
ERR_PARAM
