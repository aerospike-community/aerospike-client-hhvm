#!/bin/bash
# Run script: ./scriptname value
UserValue=$1

# loop forever
while true
do
  # start threads
  for i in $(seq 1 $UserValue)
  do
    echo "started instance no: $i"
    sh test-rest-apis.sh &
  done
  # sleep 1 second
  sleep 1
done

