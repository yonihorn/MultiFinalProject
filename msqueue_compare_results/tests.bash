#!/bin/bash

queue_benchmark.bin 2 2 100 0.5 >> changing_nthreads.txt
queue_benchmark.bin 4 2 100 0.5 >> changing_nthreads.txt
queue_benchmark.bin 8 2 100 0.5 >> changing_nthreads.txt
queue_benchmark.bin 16 2 100 0.5 >> changing_nthreads.txt
queue_benchmark.bin 32 2 100 0.5 >> changing_nthreads.txt
queue_benchmark.bin 64 2 100 0.5 >> changing_nthreads.txt
queue_benchmark.bin 80 2 100 0.5 >> changing_nthreads.txt
queue_benchmark.bin 100 2 100 0.5 >> changing_nthreads.txt
queue_benchmark.bin 80 2 100 0.5 >> changing_nthreads.txt


queue_benchmark.bin 20 2 1 0.5 >> changing_noperations.txt
queue_benchmark.bin 20 2 10 0.5 >> changing_noperations.txt
queue_benchmark.bin 20 2 50 0.5 >> changing_noperations.txt
queue_benchmark.bin 20 2 100 0.5 >> changing_noperations.txt
queue_benchmark.bin 20 2 200 0.5 >> changing_noperations.txt
queue_benchmark.bin 20  2 400 0.5 >> changing_noperations.txt
queue_benchmark.bin 20 2 600 0.5 >> changing_noperations.txt
queue_benchmark.bin 20 2 800 0.5 >> changing_noperations.txt
queue_benchmark.bin 20 2 1000 0.5 >> changing_noperations.txt
queue_benchmark.bin 20 2 2000 0.5 >> changing_noperations.txt
queue_benchmark.bin 20 2 1000 0.5 >> changing_noperations.txt


queue_benchmark.bin 20 2 100 0.2 >> changing_ratio.txt
queue_benchmark.bin 20 2 100 0.4 >> changing_ratio.txt
queue_benchmark.bin 20 2 100 0.6 >> changing_ratio.txt
queue_benchmark.bin 20 2 100 0.5 >> changing_ratio.txt
queue_benchmark.bin 20 2 100 0.8 >> changing_ratio.txt
queue_benchmark.bin 20 2 100 0.9 >> changing_ratio.txt
queue_benchmark.bin 20 2 100 1 >> changing_ratio.txt
queue_benchmark.bin 20 2 100 0.1 >> changing_ratio.txt
queue_benchmark.bin 20 2 100 0.5 >> changing_ratio.txt

queue_benchmark.bin 50 2 1 0.5 >> changing_noperations_morethreads.txt
queue_benchmark.bin 50 2 10 0.5 >> changing_noperations_morethreads.txt
queue_benchmark.bin 50 2 50 0.5 >> changing_noperations_morethreads.txt
queue_benchmark.bin 50 2 100 0.5 >> changing_noperations_morethreads.txt
queue_benchmark.bin 50 2 200 0.5 >> changing_noperations_morethreads.txt
queue_benchmark.bin 50  2 400 0.5 >> changing_noperations_morethreads.txt
queue_benchmark.bin 50 2 600 0.5 >> changing_noperations_morethreads.txt
queue_benchmark.bin 50 2 800 0.5 >> changing_noperations_morethreads.txt
queue_benchmark.bin 50 2 1000 0.5 >> changing_noperations_morethreads.txt
queue_benchmark.bin 50 2 2000 0.5 >> changing_noperations_morethreads.txt
queue_benchmark.bin 50 2 1000 0.5 >> changing_noperations_morethreads.txt

queue_benchmark.bin 50 2 100 0.2 >> changing_ratio_morethreads.txt
queue_benchmark.bin 50 2 100 0.4 >> changing_ratio_morethreads.txt
queue_benchmark.bin 50 2 100 0.6 >> changing_ratio_morethreads.txt
queue_benchmark.bin 50 2 100 0.5 >> changing_ratio_morethreads.txt
queue_benchmark.bin 50 2 100 0.8 >> changing_ratio_morethreads.txt
queue_benchmark.bin 50 2 100 0.9 >> changing_ratio_morethreads.txt
queue_benchmark.bin 50 2 100 1 >> changing_ratio_morethreads.txt
queue_benchmark.bin 50 2 100 0.1 >> changing_ratio_morethreads.txt
queue_benchmark.bin 50 2 100 0.5 >> changing_ratio_morethreads.txt