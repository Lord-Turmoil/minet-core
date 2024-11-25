#!/bin/bash

# This script is used to run the minet demo.

function run_server() {
    # Build the project
    echo -e "\033[0;33mBuilding project...\033[0m"
    ./scripts/build.sh release >/dev/null
    if [ $? -ne 0 ]; then
        echo "Failed to build the project"
        exit 1
    fi

    BIN=./build-release/demo/minet-demo
    ARGS=${1:-"demo/appsettings.jsonc"}
    if [ ! -f "$BIN" ]; then
        echo "Binary file not found: $BIN"
        exit 1
    fi
    echo -e "\033[0;36m$BIN $ARGS\033[0m"
    $BIN $ARGS
}

function run_client() {
    ROUND=${1:-10}
    BASE_URL="localhost:5000"

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
