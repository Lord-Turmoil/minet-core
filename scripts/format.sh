#!/bin/bash

find minet/include minet/src tests -name '*.cpp' -o -name '*.h' \
    | grep 'doctest.h' -v \
    | xargs clang-format -i -style=file
