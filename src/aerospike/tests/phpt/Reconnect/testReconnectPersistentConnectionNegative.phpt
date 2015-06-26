--TEST--
Basic Reconnect on invalid connection(Random IP:PORT) for persistent connection.

--SKIPIF--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_skipif("Reconnect", "testReconnectPersistentConnectionNegative");

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("Reconnect", "testReconnectPersistentConnectionNegative");
--EXPECT--
ERR_CLIENT

