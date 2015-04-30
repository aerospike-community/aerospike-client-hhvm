--TEST--
 Get a record from DB, POLICY_KEY_DIGEST and read timeout is passed in
 options.

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("Get", "testGetWithPolicyKeyDigestAndReadTimeout");
--XFAIL--
Fails due to filter bins support is not present for get().
--EXPECT--
OK
