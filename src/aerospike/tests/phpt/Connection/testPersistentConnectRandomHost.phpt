--TEST--
Connection - Persistent connect, Config with random host(IP:PORT)

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("Connection", "testPersistentConnectRandomHost");
--EXPECT--
ERR_CLUSTER
