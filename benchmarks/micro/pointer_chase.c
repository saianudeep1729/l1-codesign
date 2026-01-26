// pointer_chase.c
// Purpose: measure *true* memory latency sensitivity by defeating HW prefetch.
// Sensitivities: L1D hit latency, miss penalty behavior, TLB, MLP limits.
//
// Usage:
//   ./pointer_chase [bytes] [iters]
// bytes should be a multiple of 64 and ideally large to cross L1/L2/L3 boundaries.

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

static inline void compiler_barrier(void) {
    __asm__ __volatile__("" ::: "memory");
}

int main(int argc, char** argv) {
    size_t bytes = (argc > 1) ? (size_t)strtoull(argv[1], 0, 10) : (8ull * 1024 * 1024); // 8MB
    size_t iters = (argc > 2) ? (size_t)strtoull(argv[2], 0, 10) : (200ull * 1000 * 1000); // 200M steps

    if (bytes < 4096) bytes = 4096;
    bytes = (bytes + 63) & ~63ull;

    const size_t align = 64;
    uint64_t* arr = NULL;
    if (posix_memalign((void**)&arr, align, bytes) != 0) return 1;

    size_t n = bytes / sizeof(uint64_t);
    if (n < 2) n = 2;

    // Build a single-cycle permutation (random-ish) to defeat stride prefetch.
    // A simple xorshift-based permutation.
    size_t* perm = (size_t*)malloc(n * sizeof(size_t));
    if (!perm) return 1;
    for (size_t i = 0; i < n; i++) perm[i] = i;

    uint64_t seed = 0x123456789abcdef0ull;
    for (size_t i = n - 1; i > 0; i--) {
        seed ^= seed << 13; seed ^= seed >> 7; seed ^= seed << 17;
        size_t j = (size_t)(seed % (i + 1));
        size_t tmp = perm[i]; perm[i] = perm[j]; perm[j] = tmp;
    }

    // Link list: arr[perm[i]] = perm[i+1]
    for (size_t i = 0; i < n - 1; i++) arr[perm[i]] = (uint64_t)perm[i + 1];
    arr[perm[n - 1]] = (uint64_t)perm[0];

    compiler_barrier();

    // Chase
    uint64_t idx = (uint64_t)perm[0];
    for (size_t k = 0; k < iters; k++) {
        idx = arr[idx];
    }

    printf("%llu\n", (unsigned long long)idx);

    free(perm);
    free(arr);
    return 0;
}
