#!/bin/bash

test_file=test_result_baskets_nthreads.txt
rm $test_file
baskets_benchmark_failures.bin 2 2 10000 0.5 >> $test_file
baskets_benchmark_failures.bin 4 2 10000 0.5 >> $test_file
baskets_benchmark_failures.bin 8 2 10000 0.5 >> $test_file
baskets_benchmark_failures.bin 10 2 10000 0.5 >> $test_file
baskets_benchmark_failures.bin 12 2 10000 0.5 >> $test_file
baskets_benchmark_failures.bin 14 2 10000 0.5 >> $test_file
baskets_benchmark_failures.bin 16 2 10000 0.5 >> $test_file



test_file=test_result_baskets_1threads.txt
rm $test_file
baskets_benchmark_failures.bin 1 4 10 0.5 >> $test_file
baskets_benchmark_failures.bin 1 4 100 0.5 >> $test_file
baskets_benchmark_failures.bin 1 4 1000 0.5 >> $test_file
baskets_benchmark_failures.bin 1 4 10000 0.5 >> $test_file
baskets_benchmark_failures.bin 1 4 100000 0.5 >> $test_file
baskets_benchmark_failures.bin 1 4 1000000 0.5 >> $test_file
baskets_benchmark_failures.bin 1 4 10000000 0.5 >> $test_file

