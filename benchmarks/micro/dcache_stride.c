// dcache_stride.c
// Purpose: stress L1D via controlled stride over an array.
// Sensitivities: line size, associativity (conflicts), prefetch behavior.
//
// Usage:
//   ./dcache_stride [bytes] [stride_bytes] [passes]
// Defaults are chosen to land around L1/L2 boundaries on many CPUs.

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

static inline uint64_t rdtsc_like_fallback(uint64_t x) {
    // prevent compiler from optimizing too aggressively
    return (x * 11400714819323198485ull) ^ (x >> 17);
}

int main(int argc, char** argv) {
    size_t bytes = (argc > 1) ? (size_t)strtoull(argv[1], 0, 10) : (256ull * 1024); // 256KB
    size_t stride = (argc > 2) ? (size_t)strtoull(argv[2], 0, 10) : 64;             // 64B
    size_t passes = (argc > 3) ? (size_t)strtoull(argv[3], 0, 10) : 2000;

    if (stride == 0) stride = 64;
    if (bytes < stride * 2) bytes = stride * 2;

    // aligned alloc
    const size_t align = 64;
    uint8_t* base = NULL;
#if defined(_MSC_VER)
    base = (uint8_t*)_aligned_malloc(bytes, align);
    if (!base) return 1;
#else
    if (posix_memalign((void**)&base, align, bytes) != 0) return 1;
#endif

    // initialize to force allocation and avoid zero-page effects
    for (size_t i = 0; i < bytes; i += 64) base[i] = (uint8_t)(i);

    volatile uint64_t sink = 0;

    // Stride-walk: touches one byte per stride
    // Note: using volatile sink to prevent DCE and keep loads "observable".
    for (size_t p = 0; p < passes; p++) {
        uint64_t acc = (uint64_t)p + 1;
        for (size_t off = 0; off < bytes; off += stride) {
            acc += base[off];
            acc = rdtsc_like_fallback(acc);
        }
        sink ^= acc;
    }

    printf("%llu\n", (unsigned long long)sink);

#if defined(_MSC_VER)
    _aligned_free(base);
#else
    free(base);
#endif
    return 0;
}
