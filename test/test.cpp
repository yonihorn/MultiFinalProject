#include <iostream>
#include <immintrin.h>

int main(int argc, char** argv) {
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