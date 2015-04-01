<?php
/*
 * FUNCTION TO PARSE COMMAND LINE ARGS TO SCRIPT
 */

$HOST_ADDR = "localhost";
$HOST_PORT = 3000;
$WORKLOAD  = "RW";
$KEYS      = 1000000;
$ONCE      = FALSE;
$REPORT    = "php://stdout";

function parse_args() {
    $shortopts  = "";
    $shortopts .= "h::";    /* Optional host */
    $shortopts .= "p::";    /* Optional port */
    $shortopts .= "w::";    /* Optional workload */
    $shortopts .= "o";      /* Optional execute only once and quit */
    $shortopts .= "k::";    /* Optional keys */
    $shortopts .= "r::";    /* Optional report file */
    $shortopts .= "u";      /* Optional usage */

    $longopts  = array(
        "host::",           /* Optional host */
        "port::",           /* Optional port */
        "workload::",       /* Optional workload*/
        "once",             /* Optional execute only once and quit */
        "keys::",           /* Optional keys */
        "report::",         /* Optional report file */
        "usage"             /* Optional usage */
    );

    $options = getopt($shortopts, $longopts);
    return $options;
}


function set_global_opt_args() {
    $args = parse_args();
    if (isset($args["usage"]) || isset($args["u"])) {
        echo("php benchmark.php [-h<HOST IP ADDRESS>|--host=<HOST IP ADDRESS>
                                 -p<HOST PORT NUMBER>|--port=<HOST PORT NUMBER>
                                 -w<WORKLOAD TYPE=R,W or RW>|--workload=<WORKLOAD TYPE=R,W or RW>
                                 -o|--once     
                                 -k<NO. OF KEYS>|--keys<NO. OF KEYS>
                                 -r</path/for/report/file>|--report=</path/for/report/file>           
                                 -u|--usage]\n");
        exit(1);
    }

    global $HOST_ADDR, $HOST_PORT, $WORKLOAD, $KEYS, $ONCE, $REPORT;

    $HOST_ADDR = (isset($args["h"])) ? (string) $args["h"] : ((isset($args["host"])) ? (string) $args["host"] : "localhost");
    $HOST_PORT = (isset($args["p"])) ? (integer) $args["p"] : ((isset($args["port"])) ? (integer) $args["port"] : 3000);
    $WORKLOAD  = (isset($args["w"])) ? (string) $args["w"] : ((isset($args["workload"])) ? (string) $args["workload"] : "RW");
    $KEYS      = (isset($args["k"])) ? (integer) $args["k"] : ((isset($args["keys"])) ? (integer) $args["keys"] : 1000000);
    $ONCE      = (isset($args["o"])) ? TRUE : ((isset($args["once"])) ? TRUE : FALSE);
    $REPORT    = (isset($args["r"])) ? (string) $args["r"] : ((isset($args["report"])) ? (string) $args["report"] : "php://stdout");
}

function total_summary() {
    global $total_time, $total_count, $read_count, $write_count, $handle, $REPORT;

    $average_put_time = $total_time/$total_count;
    $tps = $total_count/$total_time;
    if ($REPORT != "php://stdout") {
        echo "Check the performance report: " .$REPORT ."\n";
    } else {
        echo "Performance report:\n";
    }
    fwrite($handle, "\nTOTAL TIME = " .$total_time. " seconds FOR ". $total_count. " OPERATIONS\n");
    fwrite($handle, "AVERAGE TIME PER OPERATION = ". $average_put_time. " seconds\n");
    fwrite($handle, "TOTAL READS = " .$read_count ."\n");
    fwrite($handle, "TOTAL WRITES = " .$write_count ."\n");
    fwrite($handle, "AVERAGE OPERATIONS PER SECOND = ". $tps. "\n");
    fclose($handle);
}

function put_rec($key) {
    $rec = array("key" => $key["key"]);
    $start = microtime(true);
    global $db;
    $status = $db->put($key, $rec);
    if ($status != Aerospike::OK) {
        echo "Aerospike put failed[{$db->errorno()}]: {$db->error()}\n";
        exit(1);
    }
    $end = microtime(true);
    global $write_count, $total_count;
    $write_count++;
    $total_count++;
    return ($end-$start);
}

function get_rec($key) {
    $start = microtime(true);
    global $db;
    $status = $db->get($key, $record);
    if ($status != Aerospike::OK) {
        echo "Aerospike get failed[{$db->errorno()}]: {$db->error()}\n";
        exit(1);
    }
    $end = microtime(true);
    global $read_count, $total_count;
    $read_count++;
    $total_count++;
    return ($end-$start);
}

function operation($key) {
    global $KEYS;
    $op = rand(0, 100);
    global $r, $w, $expected_read_count, $expected_write_count;
    if ($op <= $r) {
        if ($expected_read_count == 0) {
            $expected_read_count = (integer) $KEYS * (80/100);
        }
        $expected_read_count--;
        return get_rec($key);
    } else if ($op > $r) {
        if ($expected_write_count == 0) {
            $expected_write_count = (integer) $KEYS * (20/100);
        }
        $expected_write_count--;
        return put_rec($key);
    } else {
        echo "Failure\n";
        exit(1);
    }
}

$read_count = 0;
$write_count = 0;
$total_count = 0;
$total_time = 0;

$benchmark_time_after_keys = 100000;

set_global_opt_args();
$handle = fopen ($REPORT, 'a+');

$r = 80;
$w = 20;

$expected_read_count = (integer) $KEYS * (80/100);
$expected_write_count = (integer) $KEYS * (20/100);

$config = array("hosts"=>array(array("addr"=>$HOST_ADDR, "port"=>$HOST_PORT)));
$db = new Aerospike($config);

if (!$db->isConnected()) {
    echo "Aerospike failed to connect[{$db->errorno()}]: {$db->error()}\n";
    exit(1);
}

declare(ticks = 1);

function signal_handler($signal) {
    switch($signal) {
        case SIGTERM:
            total_summary();
            exit;
        case SIGKILL:
            total_summary();
            exit;
        case SIGINT:
            total_summary();
            exit;
    }
}

//pcntl_signal(SIGTERM, "signal_handler");
//pcntl_signal(SIGINT, "signal_handler");

do {
    for($i=0; $i<$KEYS; $i++) {

        $key = $db->initKey("test", "demo", "$i");
        if (!$key) {
            echo "Aerospike initKeyfailed[{$db->errorno()}]: {$db->error()}\n";
            exit(1);
        }

        if ($WORKLOAD == "W") {
            $total_time += put_rec($key);
        } else if ($WORKLOAD == "R") {
            $total_time += get_rec($key);
        } else if ($WORKLOAD == "RW") {
            $total_time += operation($key);
        } else {
            echo "Invalid workload\n";
            exit(1);
        }

        if ($i % $benchmark_time_after_keys == 0 && $i>0) {
            fwrite($handle, "\nOPERATION TIME = " .$total_time. " seconds FOR ". $i. " KEYS\n");
        }
    }
} while(!$ONCE);
total_summary();
?>
