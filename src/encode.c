#include "mylz4.h"
#include <string.h>
#include <stdint.h>

#define MINMATCH 4
#define LASTLITERALS 5
#define MFLIMIT 12
#define LZ4_MAX_OFFSET 65535
#define HASH_LOG 12
#define HASH_SIZE (1u << HASH_LOG)

static inline uint32_t hash4(const uint8_t *p) {
    uint32_t v;
    memcpy(&v, p, 4);
    return (v * 2654435761u) >> (32 - HASH_LOG);
}

size_t mylz4_encode_block(const uint8_t *src, size_t srcLen, uint8_t *dst, size_t dstCap) {
    if (srcLen < MFLIMIT + 1) return 0;

    const uint8_t *ip = src;
    const uint8_t *anchor = src;
    const uint8_t *iend = src + srcLen;
    const uint8_t *mflimit = iend - MFLIMIT;
    const uint8_t *matchlimit = iend - LASTLITERALS;
    uint8_t *op = dst;
    uint8_t *oend = dst + dstCap;
    (void)oend;

    uint32_t table[HASH_SIZE];
    memset(table, 0, sizeof(table));

    ip++;
    uint32_t forwardH = hash4(ip);

    for (;;) {
        uint32_t ref = table[forwardH];
        table[forwardH] = (uint32_t)(ip - src) + 1;
        const uint8_t *match = src + ref - 1;

        if (ref == 0 || (ip - match) > LZ4_MAX_OFFSET || memcmp(ip, match, MINMATCH) != 0) {
            ip++;
            if (ip > mflimit) break;
            forwardH = hash4(ip);
            continue;
        }

        // We found a match!
        const uint8_t *mstart = ip; // Save the start
        uint16_t offset = (uint16_t)(mstart - match); // Calculate offset BEFORE moving pointers!

        // Extend forward
        ip += MINMATCH;
        match += MINMATCH;
        while (ip < matchlimit && *ip == *match) {
            ip++; match++;
        }

        size_t litLen = (size_t)(mstart - anchor);
        size_t matchLen = (size_t)(ip - mstart);

        uint8_t *token = op++;
        uint8_t litNibble = litLen < 15 ? (uint8_t)litLen : 15;
        uint8_t matchNibble = (matchLen - MINMATCH) < 15 ? (uint8_t)(matchLen - MINMATCH) : 15;
        *token = (uint8_t)((litNibble << 4) | matchNibble);

        if (litNibble == 15) {
            size_t rem = litLen - 15;
            while (rem >= 255) { *op++ = 255; rem -= 255; }
            *op++ = (uint8_t)rem;
        }
        memcpy(op, anchor, litLen); op += litLen;

        *op++ = (uint8_t)(offset & 0xff);
        *op++ = (uint8_t)(offset >> 8);

        if (matchNibble == 15) {
            size_t rem = matchLen - MINMATCH - 15;
            while (rem >= 255) { *op++ = 255; rem -= 255; }
            *op++ = (uint8_t)rem;
        }

        anchor = ip;
        if (ip >= mflimit) break;
        forwardH = hash4(ip);
    }

    size_t litLen = (size_t)(iend - anchor);
    uint8_t *token = op++;
    uint8_t litNibble = litLen < 15 ? (uint8_t)litLen : 15;
    *token = (uint8_t)(litNibble << 4);
    
    if (litNibble == 15) {
        size_t rem = litLen - 15;
        while (rem >= 255) { *op++ = 255; rem -= 255; }
        *op++ = (uint8_t)rem;
    }
    memcpy(op, anchor, litLen); op += litLen;

    return (size_t)(op - dst);
}
