--TEST--
Aggregate - empty namespace and set

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("Aggregate", "testAggregateEmptyNamespaceSetNegative");
--EXPECT--
ERR_PARAM
