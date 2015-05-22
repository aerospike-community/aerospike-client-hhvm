--TEST--
Connection - Check instantiation with empty config array
--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("Connection", "testEmptyArray");
--EXPECT--
ERR_CLUSTER
