--TEST--
Operate with generation policy POLICY_GEN_GT and generation value.

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("Operate", "testOperateWithGenGTLesser");
--XFAIL--
Fails because exists() API is currently not implemented, Which is used to get metadata(i.e. Generation value).
--EXPECT--
ERR_RECORD_GENERATION
