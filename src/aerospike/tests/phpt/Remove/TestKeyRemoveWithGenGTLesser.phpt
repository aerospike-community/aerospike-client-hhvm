--TEST--
KeyRemove with generation policy POLICY_GEN_GT and generation value.

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("Remove", "testKeyRemoveWithGenGTLesser");
--EXPECT--
ERR_RECORD_GENERATION
