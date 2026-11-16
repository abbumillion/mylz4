#ifndef MYLZ4_H
#define MYLZ4_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

size_t mylz4_compress_bound(size_t srcLen);
size_t mylz4_compress(const uint8_t *src, size_t srcLen, uint8_t *dst, size_t dstCap);
size_t mylz4_decompress(const uint8_t *src, size_t srcLen, uint8_t *dst, size_t dstCap);

#ifdef __cplusplus
}
#endif
#endif
