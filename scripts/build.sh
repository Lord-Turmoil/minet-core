#!/bin/bash

flag=""
if [ "$1" == "-g" ]; then
    profile="build-debug"
    flag="$flag -DCMAKE_BUILD_TYPE=Debug"
else
    profile="build-release"
    flag="$flag -DCMAKE_BUILD_TYPE=Release"
fi

# if profile directory does not exist, create it
if [ ! -d $profile ]; then
    mkdir $profile
fi

# enter profile directory
cd $profile

# run cmake
cmake .. $flag

# build the project
make -j $(nproc)

# exit profile directory
cd ..
