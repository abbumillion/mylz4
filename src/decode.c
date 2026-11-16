#include "mylz4.h"
#include <string.h>
#include <immintrin.h> // AVX2 intrinsics

// Fast copy for literals (no overlap)
static inline void wildcopy_avx2(const uint8_t *src, uint8_t *dst, size_t len) {
    while (len >= 32) {
        __m256i v = _mm256_loadu_si256((const __m256i*)src);
        _mm256_storeu_si256((__m256i*)dst, v);
        src += 32; dst += 32; len -= 32;
    }
    while (len > 0) {
        *dst++ = *src++;
        len--;
    }
}

// Fast copy for matches (handles overlap)
static inline void matchcopy_avx2(const uint8_t *match, uint8_t *op, size_t len) {
    if (len >= 32) {
        __m256i v0 = _mm256_loadu_si256((const __m256i*)match);
        _mm256_storeu_si256((__m256i*)op, v0);
        if (len > 32) {
            __m256i v1 = _mm256_loadu_si256((const __m256i*)(match + 32));
            _mm256_storeu_si256((__m256i*)(op + 32), v1);
        }
        // For large blocks, just keep copying 32 bytes at a time
        size_t i = 64;
        while (i < len) {
            __m256i v = _mm256_loadu_si256((const __m256i*)(match + i));
            _mm256_storeu_si256((__m256i*)(op + i), v);
            i += 32;
        }
    } else {
        // Small matches fallback
        for (size_t i = 0; i < len; i++) op[i] = match[i];
    }
}

size_t mylz4_decompress(const uint8_t *src, size_t srcLen, uint8_t *dst, size_t dstCap) {
    const uint8_t *ip = src;
    const uint8_t *iend = src + srcLen;
    uint8_t *op = dst;
    uint8_t *oend = dst + dstCap;

    while (ip < iend) {
        uint8_t token = *ip++;

        // 1. Decode Literals
        size_t litLen = token >> 4;
        if (litLen == 15) {
            uint8_t b;
            do {
                if (ip >= iend) return 0;
                b = *ip++;
                litLen += b;
            } while (b == 255);
        }

        if ((size_t)(iend - ip) < litLen) return 0;
        if ((size_t)(oend - op) < litLen) return 0;

        // Copy Literals using AVX2
        wildcopy_avx2(ip, op, litLen);
        ip += litLen;
        op += litLen;

        // End of block
        if (ip >= iend) break;

        // 2. Decode Match
        if ((size_t)(iend - ip) < 2) return 0;
        uint16_t offset = (uint16_t)(ip[0] | (ip[1] << 8));
        ip += 2;

        if (offset == 0 || (size_t)(op - dst) < offset) return 0;

        size_t matchLen = token & 0x0F;
        if (matchLen == 15) {
            uint8_t b;
            do {
                if (ip >= iend) return 0;
                b = *ip++;
                matchLen += b;
            } while (b == 255);
        }
        matchLen += 4;

        if ((size_t)(oend - op) < matchLen) return 0;

        // Copy Match using AVX2
        const uint8_t *match = op - offset;
        matchcopy_avx2(match, op, matchLen);
        op += matchLen;
    }

    return (size_t)(op - dst);
}
