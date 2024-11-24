#!/bin/bash

# This script is used to run the minet demo.

# Build the project
echo -e "\033[0;33mBuilding project...\033[0m"
./scripts/build.sh >/dev/null
if [ $? -ne 0 ]; then
    echo "Failed to build the project"
    exit 1
fi

BIN=./build/demo/minet-demo
ARGS=${1:-"demo/appsettings.json"}
if [ ! -f "$BIN" ]; then
    echo "Binary file not found: $BIN"
    exit 1
fi
echo -e "\033[0;36m$BIN $ARGS\033[0m"
$BIN $ARGS
