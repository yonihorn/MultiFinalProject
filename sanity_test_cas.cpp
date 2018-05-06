#include <iostream>
#include <atomic>
#include <cassert>
#include <immintrin.h>


int test_cas() {
    std::atomic<unsigned int> variable;
    unsigned int old_true = 1;
    unsigned int old_false = 3;
    unsigned int new_ = 2;
    variable.store(1);
    std::atomic_compare_exchange_strong<unsigned int>(&variable, &old_false, 2);
    assert(variable.load(std::memory_order_relaxed) == 1);
    std::atomic_compare_exchange_strong<unsigned int>(&variable, &old_true, 2);
    std::cout << variable.load(std::memory_order_relaxed) << std::endl;
    assert(variable.load(std::memory_order_relaxed) == 2);

}

int main(int argc, char** argv) {
    test_cas();
    unsigned status;
    if ((status = _xbegin() == _XBEGIN_STARTED)) {
        std::cout << "A" << std::endl;
        _xend();
    } else {
        std::cout << "Fallback - got " << status << std::endl;
    }

    int n_tries, max_tries = 1550005;
    unsigned status2 = _XABORT_EXPLICIT;

    for (n_tries = 0; n_tries < max_tries; n_tries++) 
    {
        status2 = _xbegin ();
        if (status2 == _XBEGIN_STARTED) {
            std::cout << "gonna break with " << status2 << std::endl;
            break;
        }
    }
    if (status2 == _XBEGIN_STARTED) 
    {
        std::cout << "A2" << std::endl;
        _xend ();
    } 
    else 
    {
        std::cout << "Fallback2 - got " << status2 << std::endl;
    }
}

