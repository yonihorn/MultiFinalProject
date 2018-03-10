#include "htm_cas.h"
#include <iostream>
#include <thread>
#include <stdlib.h>
#include <algorithm>
#include <chrono>
#include <vector>
#include <atomic>

using namespace std;
typedef unsigned int u_int;

/*
Call htm cas n times, knowing it is supposed to fail as the expected value is difference from out object.
*/
bool call_htm_cas_n_times(u_int n)
{
    u_int var = 1;
    u_int different_var = 2;
    for (u_int i = 0; i < n; i++)
    {
        // CAS does not supposed to be success (as we compare two different values)
        if (htm_compare_and_swap(&var, &different_var, (u_int)100))
        {
            std::cout << "cas failed!" << std::endl;
            return false;
        }
    }
    return true;
}

/*
Call std cas n times, knowing it is supposed to fail as the expected value is difference from out object.
*/
bool call_std_cas_n_times(u_int n)
{
    std::atomic<u_int> var(1);
    u_int different_var = 2;
    u_int desired = 10;
    for (u_int i = 0; i < n; i++)
    {
        // CAS does not supposed to be success (as we compare two different values)
        if (std::atomic_compare_exchange_strong(&var, &different_var, desired))
        {
            std::cout << "cas failed!" << std::endl;
            return false;
        }
    }
    return true;
}

/*
Test the specific cas operation performance by running multiple threads that call CAS multiple times.
*/
void test_cas_failure_performance(bool(*cas_function)(u_int), u_int threads_number, u_int cas_tries)
{
    std::vector<std::thread> threads;
    u_int n = cas_tries / threads_number;
    for (u_int i = 0; i < threads_number; i++)
    {
        if (i == threads_number - 1)
        {
            n += cas_tries % threads_number;
        }
        threads.push_back(thread(call_htm_cas_n_times, n));
    }
    // wait for all threads to finish
    for (auto& t : threads)
    {
        t.join();
    }

}
/*
Command line arguments:
* Number of threads
* Number of cas to perform in each thread
*/
int main(int argc, char* argv[])
{
    // number of command line parameters should be 3
    if (argc != 3)
    {
        std::cerr << "expected 3 paramerts" << std::endl;
        return -1;
    }

    u_int threads_number = atoi(argv[1]);
    u_int cas_tries = atoi(argv[2]);

    threads_number = min(threads_number, cas_tries);

    auto t1 = std::chrono::high_resolution_clock::now();
    test_cas_failure_performance(&call_htm_cas_n_times, threads_number, cas_tries);
    auto t2 = std::chrono::high_resolution_clock::now();
    auto microseconds_elapsed = chrono::duration_cast<std::chrono::microseconds>(t2 - t1);
    // print time of htm cas in microseconds
    std::cout << "htm cas: " << microseconds_elapsed.count() << std::endl;


    t1 = std::chrono::high_resolution_clock::now();
    test_cas_failure_performance(&call_std_cas_n_times, threads_number, cas_tries);
    t2 = std::chrono::high_resolution_clock::now();
    microseconds_elapsed = chrono::duration_cast<std::chrono::microseconds>(t2 - t1);
    // print time of std cas in microseconds
    std::cout << "std cas: " << microseconds_elapsed.count() << std::endl;

    return 0;

}