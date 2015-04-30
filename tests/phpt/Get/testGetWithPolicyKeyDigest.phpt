--TEST--
 Get a record from DB, POLICY_KEY_DIGEST is passed as a option. 

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("Get", "testGetWithPolicyKeyDigest");
--XFAIL--
Fails due to filter bins support is not present for get().
--EXPECT--
OK
