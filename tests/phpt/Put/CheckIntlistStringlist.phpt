--TEST--
Put - PUT With Int and string List Combine

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("Put", "testIntlistStringlist");
--EXPECT--
OK
