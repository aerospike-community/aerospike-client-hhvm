#!/bin/bash
# Run script: ./scriptname

num_conn=500
rate=10
httperf --server localhost --port 80 --uri / --print-reply --num-conn $num_conn --rate $rate
httperf --server localhost --port 80 --uri /persistent --print-reply --num-conn $num_conn --rate $rate
httperf --server localhost --port 80 --uri /nonpersistent --print-reply --num-conn $num_conn --rate $rate
httperf --server localhost --port 80 --uri /put --print-reply --num-conn $num_conn --rate $rate --method=POST
httperf --server localhost --port 80 --uri /get --print-reply --num-conn $num_conn --rate $rate
httperf --server localhost --port 80 --uri /multiput --print-reply --num-conn $num_conn --rate $rate --method=POST
httperf --server localhost --port 80 --uri /multiget --print-reply --num-conn $num_conn --rate $rate
httperf --server localhost --port 80 --uri /getmany --print-reply --num-conn $num_conn --rate $rate
httperf --server localhost --port 80 --uri /existsmany --print-reply --num-conn $num_conn --rate $rate
httperf --server localhost --port 80 --uri /increment --print-reply --num-conn $num_conn --rate $rate --method=PUT
httperf --server localhost --port 80 --uri /multiappend --print-reply --num-conn $num_conn --rate $rate --method=PUT
httperf --server localhost --port 80 --uri /multiprepend --print-reply --num-conn $num_conn --rate $rate --method=PUT
httperf --server localhost --port 80 --uri /multitouch --print-reply --num-conn $num_conn --rate $rate --method=PUT
httperf --server localhost --port 80 --uri /removebin --print-reply --num-conn $num_conn --rate $rate --method=PUT
httperf --server localhost --port 80 --uri /remove --print-reply --num-conn $num_conn --rate $rate --method=PUT
httperf --server localhost --port 80 --uri /scan --print-reply --num-conn $num_conn --rate $rate
httperf --server localhost --port 80 --uri /scanApply --print-reply --num-conn $num_conn --rate $rate
httperf --server localhost --port 80 --uri /closeconnection --print-reply --num-conn $num_conn --rate $rate --method=PUT
httperf --server localhost --port 80 --uri /reconnect --print-reply --num-conn $num_conn --rate $rate
httperf --server localhost --port 80 --uri /getmany --print-reply --num-conn $num_conn --rate $rate
httperf --server localhost --port 80 --uri /closeconnection --print-reply --num-conn $num_conn --rate $rate --method=PUT

