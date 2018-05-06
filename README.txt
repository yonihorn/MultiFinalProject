Our project includes three main parts:
- HTM-CAS implementation. (htm_cas.h)
the CAS comparing code is in cas_compare.cpp, and the results are in 'cas_compare_results' dir.
- msqueue implementation using HTM-CAS and STD-CAS. it's in msqueue.h, and the benchmark is in queue_benchmark.cpp.
propper makefile is makefile_queue (remove the '_queue' to make)
- Basketsqueue implementation using HTM-CAS and STD-CAS. They are in basketsqueue_std.h, basketsqueue_htm.h.
Some sanity (and more) tests are in basketqueue_tests.cpp, and the benchmark is in baskets_benchmark.cpp.
propper makefile is makefile_baskets (remove the '_baskets' to make)
