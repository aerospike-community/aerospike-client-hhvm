--TEST--
Aggregate - where containing null string

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("Aggregate", "testAggregateWithWhereContainingNullString");
--EXPECT--
ERR_PARAM

