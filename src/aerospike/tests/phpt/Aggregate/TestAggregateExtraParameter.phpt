--TEST--
Aggregate - extra parameter

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("Aggregate", "testAggregateExtraParameter");
--EXPECT--
OK
