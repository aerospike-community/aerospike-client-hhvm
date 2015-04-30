--TEST--
PUT with generation policy POLICY_EXISTS_REPLACE negative.

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("Put", "testPutGenPolicyExistsReplaceNegative");
--XFAIL--
Fails because record inserted by previous test cases is not being removed by
teardown due to absence of remove() API.
--EXPECT--
ERR_RECORD_NOT_FOUND
