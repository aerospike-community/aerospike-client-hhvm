--TEST--
createIndex and dropIndex - index with same name on different string bin recreated.

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("Index", "testCreateAndDropIndexDifferentStringBinWithSameNamePositive");
--EXPECT--
OK

