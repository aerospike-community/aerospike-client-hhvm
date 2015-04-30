--TEST--
PUT with generation policy POLICY_EXISTS_CREATE positive

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("Put", "testPutGenPolicyExistsCreatePositive");
--XFAIL--
Fails because record inserted by previous test cases is not being removed by
teardown due to absence of remove() API.
--EXPECT--
OK
