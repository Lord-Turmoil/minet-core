#!/bin/bash

# This script send requests to the demo.

ROUND=${1:-10}
BASE_URL="localhost:5000"

start=`date +%s`
for (( i = 1; i <= $ROUND; i++ )); do
    curl -s $BASE_URL/ping
    rand=$(( RANDOM % 2 ))  # sleep 0-1 seconds
    sleep $rand
done
end=`date +%s`
echo "Sent $ROUND requests in $((end-start)) seconds"
