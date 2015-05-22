--TEST--
Reconnect - Check for Basic Reconnect after close for a non-persistent connection.

--SKIPIF--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_skipif("Reconnect", "testReconnectNonPersistentConnectionPositive");

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("Reconnect", "testReconnectNonPersistentConnectionPositive");
--EXPECT--
OK

