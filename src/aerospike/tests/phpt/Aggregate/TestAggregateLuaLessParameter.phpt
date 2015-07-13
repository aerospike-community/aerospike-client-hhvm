--TEST--
Aggregate - less parameters in udf

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("Aggregate", "testAggregateLuaLessParameter");
--EXPECTREGEX--
(OK|ERR_CLIENT)
