#include <immintrin.h>
#include <cstring>

template <class T> 
inline bool htm_compare_and_swap(T* obj, T* expected, T desired) {
    unsigned status;
    if (std::memcmp(obj, expected, sizeof(T)) == 0) {
        if (status = _xbegin() == _XBEGIN_STARTED) {
            if (std::memcmp(obj, expected, sizeof(T)) == 0) {
                std::memcpy(obj, &desired, sizeof(T));
                _xend();
                return true;
            } else {
                _xend();
            }
        }
    }
    return false;
}

template<>
inline bool htm_compare_and_swap<u_int>(u_int* obj, u_int* expected, u_int desired) {
    unsigned status;
    if (*obj == *expected) {
        if (status = _xbegin() == _XBEGIN_STARTED) {
            if (*obj == *expected) {
                *obj = desired;
                _xend();
                return true;
            } else {
                _xend();
            }
        }
    }
}