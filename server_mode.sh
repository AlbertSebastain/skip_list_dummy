#!/bin/bash
if [ -d"./build" ]; then
    rm -r build
fi
echo "# define SERVER" > config.h.in
mkdir build
cd ./build
cmake  -DSERVER_MODE=true ..
make
echo "successful"
./skiplist_server