--TEST--
Aggregate - extra parameter in lua

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("Aggregate", "testAggregateLuaExtraParameter");
--EXPECTF--
OK
