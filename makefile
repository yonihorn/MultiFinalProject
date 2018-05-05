all:
	g++ msqueue.h -mrtm -o msqueue.bin -std=c++11
	g++ htm_cas.h -mrtm -o stm_cas.bin -std=c++11
	g++ basketsqueue.h -mrtm -o basketsqueue.bin -std=c++11
	g++ basketsqueue_htm.h -mrtm -o basketsqueue_htm.bin -std=c++11
	g++ basketsqueue_std.h -mrtm -o basketsqueue_std.bin -std=c++11
	g++ example.cpp -mrtm -o example.bin -std=c++11
	g++ cas_compare.cpp -mrtm -o cas_compare.bin -pthread -std=c++11
<<<<<<< HEAD
	g++ basketsqueue.cpp -mrtm -o basketsqueue.bin -pthread -latomic -std=c++11
=======
	g++ queue_benchmark.cpp -mrtm -o queue_benchmark.bin -pthread -std=c++11
>>>>>>> benchmark
