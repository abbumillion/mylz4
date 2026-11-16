#include "mylz4.h"

// Declaration of the function we just wrote in encode.c
size_t mylz4_encode_block(const uint8_t *src, size_t srcLen, uint8_t *dst, size_t dstCap);

size_t mylz4_compress_bound(size_t srcLen) { 
    return srcLen + (srcLen/255) + 16; 
}

size_t mylz4_compress(const uint8_t *src, size_t srcLen, uint8_t *dst, size_t dstCap) {
    return mylz4_encode_block(src, srcLen, dst, dstCap);
}
