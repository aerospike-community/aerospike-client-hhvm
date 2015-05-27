--TEST--
Connection - Non-persistent connect, Config with random host(IP:PORT)
--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("Connection", "testNonPersistentConnectRandomHost");
--EXPECT--
ERR_CLUSTER
