#!/bin/bash
# Run script: ./scriptname

num_conn=100
rate=10
httperf --server localhost --port 80 --uri / --print-reply --num-conn $num_conn --rate $rate
httperf --server localhost --port 80 --uri /persistent --print-reply --num-conn $num_conn --rate $rate
httperf --server localhost --port 80 --uri /nonpersistent --print-reply --num-conn $num_conn --rate $rate
httperf --server localhost --port 80 --uri /put --print-reply --num-conn $num_conn --rate $rate --method POST
httperf --server localhost --port 80 --uri /get --print-reply --num-conn $num_conn --rate $rate
httperf --server localhost --port 80 --uri /multiput --print-reply --num-conn $num_conn --rate $rate --method POST
httperf --server localhost --port 80 --uri /multiget --print-reply --num-conn $num_conn --rate $rate
httperf --server localhost --port 80 --uri /getmany --print-reply --num-conn $num_conn --rate $rate
httperf --server localhost --port 80 --uri /existsmany --print-reply --num-conn $num_conn --rate $rate
httperf --server localhost --port 80 --uri /append --print-reply --num-conn $num_conn --rate $rate --method PUT
httperf --server localhost --port 80 --uri /multiappend --print-reply --num-conn $num_conn --rate $rate --method PUT
httperf --server localhost --port 80 --uri /prepend --print-reply --num-conn $num_conn --rate $rate --method PUT
httperf --server localhost --port 80 --uri /multiprepend --print-reply --num-conn $num_conn --rate $rate --method PUT
httperf --server localhost --port 80 --uri /increment --print-reply --num-conn $num_conn --rate $rate --method PUT
httperf --server localhost --port 80 --uri /multiincrement --print-reply --num-conn $num_conn --rate $rate --method PUT
httperf --server localhost --port 80 --uri /touch --print-reply --num-conn $num_conn --rate $rate --method PUT
httperf --server localhost --port 80 --uri /multitouch --print-reply --num-conn $num_conn --rate $rate --method PUT
httperf --server localhost --port 80 --uri /scan --print-reply --num-conn $num_conn --rate $rate
httperf --server localhost --port 80 --uri /scanApply --print-reply --num-conn $num_conn --rate $rate
httperf --server localhost --port 80 --uri /query --print-reply --num-conn $num_conn --rate $rate
httperf --server localhost --port 80 --uri /register --print-reply --num-conn $num_conn --rate $rate
httperf --server localhost --port 80 --uri /listRegistered --print-reply --num-conn $num_conn --rate $rate
httperf --server localhost --port 80 --uri /getRegistered --print-reply --num-conn $num_conn --rate $rate
httperf --server localhost --port 80 --uri /apply --print-reply --num-conn $num_conn --rate $rate
httperf --server localhost --port 80 --uri /deregister --print-reply --num-conn $num_conn --rate $rate
httperf --server localhost --port 80 --uri /removebin --print-reply --num-conn $num_conn --rate $rate --method PUT
httperf --server localhost --port 80 --uri /remove --print-reply --num-conn $num_conn --rate $rate --method PUT
httperf --server localhost --port 80 --uri /closeconnection --print-reply --num-conn $num_conn --rate $rate --method PUT
httperf --server localhost --port 80 --uri /reconnect --print-reply --num-conn $num_conn --rate $rate
httperf --server localhost --port 80 --uri /closeconnection --print-reply --num-conn $num_conn --rate $rate --method PUT
