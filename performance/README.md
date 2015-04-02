## INSTRUCTIONS TO RUN THE BENCHMARK TOOL:

    hhvm -d extension_dir=../ -d hhvm.extensions[]=aerospike.so benchmark.php 
    [-h<HOST IP ADDRESS>|--host=<HOST IP ADDRESS>
     -p<HOST PORT NUMBER>|--port=<HOST PORT NUMBER>
     -w<WORKLOAD TYPE=R,W or RW>|--workload=<WORKLOAD TYPE=R,W or RW>
     -o|--once
     -k<NO. OF KEYS>|--keys<NO. OF KEYS>
     -r</path/for/report/file>|--report=</path/for/report/file>
     -u|--usage]

## SAMPLE WAYS TO EXECUTE:

# Example 1:

To perform a total of 50000 reads and writes randomly in the ratio of 80:20, execute all the operations once and quit, generating the report in an arbitrary path, use the following:

    hhvm -d extension_dir=../ -d hhvm.extensions[]=aerospike.so benchmark.php -h"localhost" -p3000 -wRW -o -k50000 -r"/tmp/benchmark_report.log" OR
    hhvm -d extension_dir=../ -d hhvm.extensions[]=aerospike.so benchmark.php --host="localhost" --port=3000 --workload=RW --once --keys=50000 --report="/tmp/benchmark_report.log"

# Example 2: 

To perform a total of 50000 reads, execute the operations infinitely until externally stopped using "Ctrl+C", generating the report on the console, use the following:

    hhvm -d extension_dir=../ -d hhvm.extensions[]=aerospike.so benchmark.php -wR -k100000

