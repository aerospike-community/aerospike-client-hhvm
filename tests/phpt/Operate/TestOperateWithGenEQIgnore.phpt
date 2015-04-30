--TEST--
Operate with generation policy POLICY_GEN_IGNORE

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("Operate", "testOperateWithGenEQIgnore");
--XFAIL--
Fails because exists() API is currently not implemented, Which is used to get metadata(i.e. Generation value).
--EXPECT--
OK

