#include "htm_cas.h"
#include <iostream>

int main() {
    unsigned int var = 1;
    unsigned int to_cmp = 1;
    if (htm_compare_and_swap(&var, &to_cmp,(unsigned int)5)) {
        std::cout << "Success: " << var << std::endl;
    } else {
        std::cout << "Failed: " << var << std::endl;
    }

    unsigned int test_var = 1;
    int failures = 0;
    int successes = 0;
    for (int i = 0; i < 100; i++) {
        if (htm_compare_and_swap(&test_var, &to_cmp, (unsigned int) 1)) {
            successes++;
        } else {
            failures ++;
        }
    }
    std::cout << "Success: " << successes << std::endl << "Failures: " << failures << std::endl;
}