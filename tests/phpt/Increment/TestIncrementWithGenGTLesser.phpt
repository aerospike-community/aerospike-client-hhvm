--TEST--
Increment with generation policy POLICY_GEN_GT and generation value.

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("Increment", "testIncrementWithGenGTLesser");
--XFAIL--
Fails because exists() API is currently not implemented, Which is used to get metadata(i.e. Generation value).
--EXPECT--
OK

