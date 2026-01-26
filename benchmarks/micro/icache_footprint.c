#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

__attribute__((noinline)) uint64_t f(uint64_t x) {
    // intentionally simple; footprint comes from multiple functions
    return (x * 1315423911u) ^ (x >> 7);
}

int main(int argc, char** argv) {
    uint64_t iters = (argc > 1) ? strtoull(argv[1], 0, 10) : 200000000ULL;
    uint64_t x = 1;
    for (uint64_t i=0;i<iters;i++) x = f(x);
    printf("%llu\n", (unsigned long long)x);
    return 0;
}
