#!/bin/bash

flag=""
if [ "$1" == "debug" ]; then
    profile="build-debug"
    flag="$flag -DCMAKE_BUILD_TYPE=Debug"
elif [ "$1" == "release" ]; then
    profile="build-release"
    flag="$flag -DCMAKE_BUILD_TYPE=Release"
else
    profile="build"
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
