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

u_int htm_var = 1;
u_int htm_desired = 10;
/*
Call htm cas n times, knowing it is supposed to fail as the expected value is difference from out object.
*/
bool call_htm_cas_n_times(u_int n)
{
    u_int htm_different_var = 5;
    for (u_int i = 0; i < n; i++)
    {
        // CAS should fail (as we compare two different values)
        if (htm_compare_and_swap(&htm_var, &htm_different_var, htm_desired))
        {
            std::cout << "cas succeed. But is should Fail! htm_var: " << htm_var << " htm_different: " << htm_different_var << std::endl;
            return false;
        }
        // don't really need this line. but we want similar behaviour (to the STD cas)
        htm_different_var = 5;
    }
    return true;
}

/*
Call std cas n times, knowing it is supposed to fail as the expected value is difference from out object.
*/

std::atomic<u_int> std_var(1);
u_int std_desired = 10;
bool call_std_cas_n_times(u_int n)
{
    u_int std_different_var = 5;
    for (u_int i = 0; i < n; i++)
    {
        // CAS should fail (as we compare two different values)
        if (std::atomic_compare_exchange_strong(&std_var, &std_different_var, std_desired))
        {
            std::cout << "cas succeed. But is should Fail! std_var: " << std_var << " std_different: " << std_different_var << std::endl;
            return false;
        }
        // as the std CAS changes the expected
        std_different_var = 5;
    }
    return true;
}

/*
Test the specific cas operation performance by running multiple threads that call CAS multiple times.
*/
void test_cas_failure_performance(bool(*cas_function)(u_int), u_int threads_number, u_int cas_tries)
{
    std::vector<std::thread> threads;
    for (u_int i = 0; i < threads_number; i++)
    {
        threads.push_back(thread(cas_function, cas_tries));
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
* Number of cas to perform (per thread)
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
    u_int cas_tries = atoi(argv[2]); // per thread

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