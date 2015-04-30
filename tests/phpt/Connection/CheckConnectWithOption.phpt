--TEST--
Connection - Check Connect with options.

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("Connection", "testConnectWithOption");
--XFAIL--
Fails because persistent connection support is not present.
--EXPECT--
OK
