#!/bin/bash

# This script is used to run the minet demo.

# Build the project
./scripts/build.sh >/dev/null
if [ $? -ne 0 ]; then
    echo "Failed to build the project"
    exit 1
fi

BIN=./build/demo/minet-demo
if [ ! -f "$BIN" ]; then
    echo "Binary file not found: $BIN"
    exit 1
fi
$BIN $@
