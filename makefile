all:
	g++ htm_cas.h -mrtm -o stm_cas.bin
	g++ example.cpp -mrtm -o example.bin