// branchy.c
// Purpose: stress branch prediction and frontend throughput.
// Sensitivities: branch-miss penalty, I$ footprint (if code expands), BTB behavior.
//
// Usage:
//   ./branchy [iters] [pattern]
// pattern:
//   0 = predictable (always-taken-ish)
//   1 = alternating
//   2 = pseudo-random (harder predictor stress)

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

static inline uint64_t xorshift64(uint64_t x) {
    x ^= x << 13;
    x ^= x >> 7;
    x ^= x << 17;
    return x;
}

int main(int argc, char** argv) {
    uint64_t iters = (argc > 1) ? strtoull(argv[1], 0, 10) : 400000000ULL;
    int pattern = (argc > 2) ? atoi(argv[2]) : 2;

    uint64_t x = 0xdeadbeefcafebabeull;
    volatile uint64_t sink = 0;

    for (uint64_t i = 0; i < iters; i++) {
        uint64_t b;
        if (pattern == 0) {
            b = 1; // predictable
        } else if (pattern == 1) {
            b = i & 1; // alternating
        } else {
            x = xorshift64(x);
            b = x & 1; // pseudo-random
        }

        // Two dependent branches to stress predictor behavior a bit.
        if (b) sink += (i ^ x);
        else   sink ^= (i + 0x9e3779b97f4a7c15ull);

        if ((sink & 7) == 3) sink += 1;
    }

    printf("%llu\n", (unsigned long long)sink);
    return 0;
}
