#!/bin/bash

cas_compare.bin 1 100 >> changing_noperations_1_thread.txt
cas_compare.bin 1 200 >> changing_noperations_1_thread.txt
cas_compare.bin 1 500  >> changing_noperations_1_thread.txt
cas_compare.bin 1 1000  >> changing_noperations_1_thread.txt
cas_compare.bin 1 5000  >> changing_noperations_1_thread.txt
cas_compare.bin 1 10000  >> changing_noperations_1_thread.txt

cas_compare.bin 1 10000 >> changing_threads.txt
cas_compare.bin 2 10000 >> changing_threads.txt
cas_compare.bin 4 10000  >> changing_threads.txt
cas_compare.bin 6 10000  >> changing_threads.txt
cas_compare.bin 8 10000  >> changing_threads.txt
cas_compare.bin 10 10000  >> changing_threads.txt
cas_compare.bin 12 10000  >> changing_threads.txt
cas_compare.bin 16 10000  >> changing_threads.txt

cas_compare.bin 10 10 >> changing_noperations.txt
cas_compare.bin 10 50 >> changing_noperations.txt
cas_compare.bin 10 100 >> changing_noperations.txt
cas_compare.bin 10 1000 >> changing_noperations.txt
cas_compare.bin 10 5000 >> changing_noperations.txt
cas_compare.bin 10 10000 >> changing_noperations.txt
cas_compare.bin 10 50000 >> changing_noperations.txt
cas_compare.bin 10 100000 >> changing_noperations.txt
cas_compare.bin 10 500000 >> changing_noperations.txt

cas_compare.bin 25 10 >> changing_noperations_morethreads.txt
cas_compare.bin 25 50 >> changing_noperations_morethreads.txt
cas_compare.bin 25 100 >> changing_noperations_morethreads.txt
cas_compare.bin 25 1000 >> changing_noperations_morethreads.txt
cas_compare.bin 25 5000 >> changing_noperations_morethreads.txt
cas_compare.bin 25 10000 >> changing_noperations_morethreads.txt
cas_compare.bin 25 50000 >> changing_noperations_morethreads.txt
cas_compare.bin 25 100000 >> changing_noperations_morethreads.txt
cas_compare.bin 25 500000 >> changing_noperations_morethreads.txt

