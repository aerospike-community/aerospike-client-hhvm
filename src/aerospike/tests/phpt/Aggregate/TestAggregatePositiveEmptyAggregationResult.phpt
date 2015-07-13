--TEST--
Aggregate - Positive case with where predicate not satisfying any record and
hence empty aggregation.

--FILE--
<?php
include dirname(__FILE__)."/../../astestframework/astest-phpt-loader.inc";
aerospike_phpt_runtest("Aggregate", "testAggregatePositiveEmptyAggregationResult");
--EXPECT--
OK


