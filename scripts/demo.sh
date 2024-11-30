#!/bin/bash

# This script is used to run the minet demo.

function run_server() {
    PROFILE=debug
    # Build the project
    echo -e "\033[0;33mBuilding project...\033[0m"
    ./scripts/build.sh $PROFILE >/dev/null
    if [ $? -ne 0 ]; then
        echo "Failed to build the project"
        exit 1
    fi

    BIN=./build-$PROFILE/demo/minet-demo
    if [ ! -f "$BIN" ]; then
        echo "Binary file not found: $BIN"
        exit 1
    fi

    ARGS="demo/appsettings.jsonc"
    if [ "$1" == "mayhem" ]; then
        ARGS="demo/appsettings.mayhem.jsonc"
    fi
    
    echo -e "\033[0;36m$BIN $ARGS\033[0m"
    # do not check for signal-unsafe call, as it is a desired behavior
    TSAN_OPTIONS="report_signal_unsafe=0" $BIN $ARGS
}

function send_requests() {
    BASE_URL="localhost:5000"
    ROUND=${1:-10}
    start=`date +%s`
    for (( i = 1; i <= $ROUND; i++ )); do
        rand=$(( RANDOM % 2 ))  # choose 0 or 1
        if [ $rand -eq 0 ]; then
            # GET /ping
            curl -s $BASE_URL/ping
        else
            # POST /echo with random text data
            length=$(( RANDOM % 128 + 1 ))  # random length 1-100
            text=$(cat /dev/urandom | tr -dc 'a-zA-Z0-9' | fold -w $length | head -n 1)
            curl -s -X POST -H "Content-Type: text/plain" -d "$text" $BASE_URL/echo
        fi
        echo ""
        # rand=$(( RANDOM % 3 ))  # sleep 0-3 seconds
        # sleep $rand
    done
    end=`date +%s`
    echo "Sent $ROUND requests in $((end-start)) seconds"
}

function run_client() {
    if [ "$1" == "_" ]; then
        shift
        LEVEL=${1:?"Expect level"}
        ROUND=${2:?"Expect round"}
        if [ $LEVEL -gt 0 ]; then
            bash $0 client _ $((LEVEL-1)) $ROUND &
            bash $0 client _ $((LEVEL-1)) $ROUND &
        else
            send_requests $ROUND
        fi
    else
        ROUND=${1:-5}
        bash $0 client _ 2 $ROUND
    fi
}

option=$1
shift
if [ "$option" == "server" ]; then
    echo -e "\033[0;32mChoose to run server\033[0m"
    run_server $@
elif [ "$option" == "client" ]; then
    echo -e "\033[0;32mChoose to run client...\033[0m"
    run_client $@
else
    echo "Usage: $0 [server|client] [args]"
fi
