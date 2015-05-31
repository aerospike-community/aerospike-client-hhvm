--TEST--
Basic Reconnect on invalid connection(Random IP:PORT) for non-persistent connection.

--SKIPIF--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_skipif("Reconnect", "testReconnectNonPersistentConnectionNegative");

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("Reconnect",
        "testReconnectNonPersistentConnectionNegative");
--EXPECT--
ERR_CLIENT

