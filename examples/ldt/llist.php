<?php
################################################################################
# Copyright 2013-2015 Aerospike, Inc.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
################################################################################
require_once(realpath(__DIR__ . '/../examples_util.php'));

function parse_args() {
    $shortopts  = "";
    $shortopts .= "h::";  /* Optional host */
    $shortopts .= "p::";  /* Optional port */
    $shortopts .= "a";    /* Optionally annotate output with code */
    $shortopts .= "c";    /* Optionally clean up before leaving */

    $longopts  = array(
        "host::",         /* Optional host */
        "port::",         /* Optional port */
        "annotate",       /* Optionally annotate output with code */
        "clean",          /* Optionally clean up before leaving */
        "help",           /* Usage */
    );
    $options = getopt($shortopts, $longopts);
    return $options;
}

$args = parse_args();
if (isset($args["help"])) {
    echo "php llist.php [-hHOST] [-pPORT] [-a] [-c]\n";
    echo " or\n";
    echo "php llist.php [--host=HOST] [--port=PORT] [--annotate] [--clean]\n";
    exit(1);
}
$HOST_ADDR = (isset($args["h"])) ? (string) $args["h"] : ((isset($args["host"])) ? (string) $args["host"] : "localhost");
$HOST_PORT = (isset($args["p"])) ? (integer) $args["p"] : ((isset($args["port"])) ? (string) $args["port"] : 3000);

echo colorize("Connecting to the host ≻", 'black', true);
$start = __LINE__;
$config = array("hosts" => array(array("addr" => $HOST_ADDR, "port" => $HOST_PORT)));
$db = new Aerospike($config, false);
if (!$db->isConnected()) {
    echo fail("Could not connect to host $HOST_ADDR:$HOST_PORT [{$db->errorno()}]: {$db->error()}");
    exit(1);
}
echo success();
if (isset($args['a']) || isset($args['annotate'])) display_code(__FILE__, $start, __LINE__);

echo colorize("Adding a record to test.vehicles with PK=M4G000 ≻", 'black', true);
$start = __LINE__;
$key = $db->initKey("test", "vehicles", "M4G000");
$vehicle = array("make" => "Mazda", "model" => "CX-3", "year" => 2014, "color" => "red");
$options = array(Aerospike::OPT_POLICY_KEY => Aerospike::POLICY_KEY_SEND);
$status = $db->put($key, $vehicle, 0, $options);
if ($status === Aerospike::OK) {
    echo success();
} else {
    echo standard_fail($db);
}
if (isset($args['a']) || isset($args['annotate'])) display_code(__FILE__, $start, __LINE__);

echo colorize("Instantiating an LList representing bin 'rental_history' of the given record ≻", 'black', true);
$start = __LINE__;
require_once(realpath(__DIR__ . '/../../autoload.php'));
$rental_history = new \Aerospike\LDT\LList($db, $key, 'rental_history');
if ($rental_history->errorno() === Aerospike::OK) {
    echo success();
} else {
    echo standard_fail($rental_history);
}
if (isset($args['a']) || isset($args['annotate'])) display_code(__FILE__, $start, __LINE__);

echo colorize("Checking if the server actually has an LList at bin 'rental_history' of the record ≻", 'black', true);
$start = __LINE__;
if (!$rental_history->isLDT()) {
    echo fail("No LList exists yet at bin 'rental_history' of record {$key['key']}. Adding elements will initialize it.");
} else {
    echo success();
}
if (isset($args['a']) || isset($args['annotate'])) display_code(__FILE__, $start, __LINE__);

echo colorize("Add an element to the record's LList bin ≻", 'black', true);
$start = __LINE__;
$date = new DateTime();
$date->setDate(2014, 6, 18);
$rental_event = array('key' => 0, 'clientid' => 100100123, 'date' => $date->getTimestamp());
$status = $rental_history->add($rental_event);
if ($status === Aerospike::OK) {
    echo success();
} else {
    echo standard_fail($rental_history);
}
if (isset($args['a']) || isset($args['annotate'])) display_code(__FILE__, $start, __LINE__);

echo colorize("Add several other elements to the record's LList bin ≻", 'black', true);
$start = __LINE__;
$i = 1;
$values = array();
while ($i < 6) {
    $interval = new DateInterval('P7D');
    $date = $date->add($interval);
    $re_clientid = $rental_event['clientid'] + 2;
    $values[] = array('key' => $i, 'clientid' => $re_clientid, 'date' => $date->getTimestamp());
    $i++;
}
$status = $rental_history->addMany($values);
if ($status === Aerospike::OK) {
    echo success();
} else {
    echo standard_fail($rental_history);
}
if (isset($args['a']) || isset($args['annotate'])) display_code(__FILE__, $start, __LINE__);

echo colorize("Getting the full record ≻", 'black', true);
$start = __LINE__;
$status = $db->get($key, $record);
if ($status === Aerospike::OK) {
    echo success();
    var_dump($record);
} elseif ($status === Aerospike::ERR_RECORD_NOT_FOUND) {
    echo fail("Could not find a vehicle with PK={$key['key']} in the set test.vehicles");
} else {
    echo standard_fail($db);
}
if (isset($args['a']) || isset($args['annotate'])) display_code(__FILE__, $start, __LINE__);

echo colorize("Update the LList element with 'key' 0 ≻", 'black', true);
$start = __LINE__;
$date = new DateTime();
$date->setDate(2013, 7, 12);
$rental_event = array('key' => 0, 'clientid' => 100100123, 'date' => $date->getTimestamp());
$status = $rental_history->update($rental_event);
if ($status === Aerospike::OK) {
    echo success();
} else {
    echo standard_fail($rental_history);
}
if (isset($args['a']) || isset($args['annotate'])) display_code(__FILE__, $start, __LINE__);

echo colorize("Counting the elements in the record's LList bin ≻", 'black', true);
$start = __LINE__;
$status = $rental_history->size($num_elements);
if ($status === Aerospike::OK) {
    echo success();
    echo colorize("There are $num_elements elements in the LList\n", 'green');
} else {
    echo standard_fail($rental_history);
}
if (isset($args['a']) || isset($args['annotate'])) display_code(__FILE__, $start, __LINE__);

echo colorize("Get the elements for keys 0-3 in the record's LList bin ≻", 'black', true);
$start = __LINE__;
$status = $rental_history->findRange(0, 3, $elements);
if ($status === Aerospike::OK) {
    echo success();
    var_dump($elements);
} else {
    echo standard_fail($rental_history);
}
if (isset($args['a']) || isset($args['annotate'])) display_code(__FILE__, $start, __LINE__);

echo colorize("Find the LList element with 'key' 4 ≻", 'black', true);
$start = __LINE__;
$status = $rental_history->find(array('key' => 4), $elements);
if ($status === Aerospike::OK) {
    echo success();
    var_dump($elements);
} else {
    echo standard_fail($rental_history);
}
if (isset($args['a']) || isset($args['annotate'])) display_code(__FILE__, $start, __LINE__);

if (isset($args['c']) || isset($args['clean'])) {
    $start = __LINE__;
    echo colorize("Removing a range of elements with key 0-5 from the LDT ≻", 'black', true);
    $status = $rental_history->removeRange(0, 5);
    if ($status === Aerospike::OK) {
        echo success();
    } else {
        echo standard_fail($db);
    }
    if (isset($args['a']) || isset($args['annotate'])) display_code(__FILE__, $start, __LINE__);

    $start = __LINE__;
    echo colorize("Destroying the LDT ≻", 'black', true);
    $status = $rental_history->destroy();
    if ($status === Aerospike::OK) {
        echo success();
    } else {
        echo standard_fail($db);
    }
    if (isset($args['a']) || isset($args['annotate'])) display_code(__FILE__, $start, __LINE__);

    $start = __LINE__;
    echo colorize("Removing the record ≻", 'black', true);
    $status = $db->remove($key);
    if ($status === Aerospike::OK) {
        echo success();
    } else {
        echo standard_fail($db);
    }
    if (isset($args['a']) || isset($args['annotate'])) display_code(__FILE__, $start, __LINE__);

}

$db->close();
?>
