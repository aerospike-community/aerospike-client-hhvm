--TEST--
RemoveBin with generation policy POLICY_GEN_GT and generation value.

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("Remove", "testRemoveBinWithGenGTLesser");
--EXPECTREGEX--
OK|ERR_CLIENT
