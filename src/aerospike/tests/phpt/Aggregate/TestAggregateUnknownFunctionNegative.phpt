--TEST--
Aggregate - unknown function

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("Aggregate", "testAggregateUnknownFunctionNegative");
--EXPECTREGEX--
(ERR_CLIENT|ERR_UDF)
