# Faster serialization - Final Project on 0368418301 TAU

__Students:__

Udi rot

Noa koren

Yoni Hornstein

## Compile
runnnig `make` will compile all necessary executeables:

## Our project includes three main parts:
* HTM-CAS implementation. (htm_cas.h) the CAS comparing code is in cas_compare.cpp, and the results are in 'cas_compare_results' dir.
* msqueue implementation using HTM-CAS and STD-CAS. it's in msqueue.h, and the benchmark is in queue_benchmark.cpp.
* Basketsqueue implementation using HTM-CAS and STD-CAS. They are in basketsqueue_std.h, basketsqueue_htm.h.

Some sanity (and more) tests are in basketqueue_tests.cpp, and the benchmark is in baskets_benchmark.cpp.

