all:
	g++ example.cpp -mrtm -o example.bin -std=c++11
	g++ cas_compare.cpp -mrtm -o cas_compare.bin -pthread -std=c++11
	g++ queue_benchmark.cpp -mrtm -o queue_benchmark.bin -pthread -std=c++11
	g++ baskets_benchmark.cpp -mrtm -o baskets_benchmark.bin -pthread -latomic -std=c++11
	g++ baskets_benchmark_failures.cpp -mrtm -o baskets_benchmark_failures.bin -pthread -latomic -std=c++11