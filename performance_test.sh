#!/bin/bash
if [ -d "./build" ]; then
    rm -r build
fi
mkdir ./build
if [ -f "./performance_test_result" ]; then
    rm performance_test_result
fi
if [ -f "./confg.h.in" ]; then
    rm config.h.in
fi
touch config.h.in
test_fun=("INSERT" "SEARCH_FAST" "SEARCH")
num_threads=(1 10)
test_count=(10000 100000)
for fun in ${test_fun[@]}
do
    for thread in ${num_threads[@]}
    do
        for count in ${test_count[@]}
        do
            echo "#define NUM_THREADS ${thread}"  > ./config.h.in
            echo "# define TEST_COUNT ${count}" >> ./config.h.in
            echo "# define ${fun}" >> ./config.h.in
            cd ./build
            cmake -DSERVER_MODE=false ..
            make
            ./skiplist
            cd ..
        done
    done
done
