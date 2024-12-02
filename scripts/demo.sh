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
        echo -e "\033[0;36mRun with Mayhem server...\033[0m"
        ARGS="demo/appsettings.mayhem.json"
    elif [ "$1" == "threaded" ]; then
        echo -e "\033[0;36mRun with Threaded server...\033[0m"
        ARGS="demo/appsettings.threaded.json"
    fi
    
    echo -e "\033[0;36m$BIN $ARGS\033[0m"
    # do not check for signal-unsafe call, as it is a desired behavior
    TSAN_OPTIONS="report_signal_unsafe=0" $BIN $ARGS
}

function send_requests() {
    BASE_URL="localhost:5000"
    ROUND=${1:-10}
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
    done
}

function run_client() {
    if [ "$1" == "_" ]; then
        shift
        LEVEL=${1:?"Expect level"}
        ROUND=${2:?"Expect round"}
        if [ $LEVEL -gt 0 ]; then
            bash $0 client _ $((LEVEL-1)) $ROUND &
            c1=$!
            bash $0 client _ $((LEVEL-1)) $ROUND &
            c2=$!
            wait $c1
            wait $c2
        else
            send_requests $ROUND
        fi
    else
        M_ROUND=${1:-10}
        M_LEVEL=${2:-2}
        start=`date +%s`
        
        bash $0 client _ $M_LEVEL $M_ROUND
        wait $!
        
        end=`date +%s`
        echo "Sent $((M_ROUND*(2**M_LEVEL))) ($M_ROUND * $((2**$M_LEVEL))) requests in $((end-start)) seconds"
    fi
}

option=$1
shift
if [ "$option" == "server" ]; then
    echo -e "\033[0;32mChoose to run server\033[0m"
    run_server $@
elif [ "$option" == "client" ]; then
    if [ "$1" != "_" ]; then
        echo -e "\033[0;32mChoose to run client...\033[0m"
    fi
    run_client $@
else
    echo "Usage: $0 [server|client] [args]"
    echo "  server          - run with Basic server"
    echo "  server threaded - run with Threaded server"
    echo "  server mayhem   - run with Mayhem server"
    echo "  client N        - 4 processes, each sends N requests"
fi
