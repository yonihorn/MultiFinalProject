#include <immintrin.h>
#include <cstring>

template <class T> 
bool htm_compare_and_swap(T* obj, T* expected, T desired) {
    unsigned status;
    if (status = _xbegin() == _XBEGIN_STARTED) {
        if (std::memcmp(obj, expected, sizeof(T)) == 0) {
            std::memcpy(obj, &desired, sizeof(T));
            _xend();
            return true;
        } else {
            _xend();
        }
    }
    
    return false;
}