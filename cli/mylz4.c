#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mylz4.h"

static uint8_t *read_file(const char *path, size_t *outLen) {
    FILE *f = fopen(path, "rb");
    if (!f) return NULL;
    fseek(f, 0, SEEK_END);
    long sz = ftell(f);
    fseek(f, 0, SEEK_SET);
    
    uint8_t *buf = malloc((size_t)sz + 32);
    if (fread(buf, 1, (size_t)sz, f) != (size_t)sz) {
        free(buf); fclose(f); return NULL;
    }
    fclose(f);
    *outLen = (size_t)sz;
    return buf;
}

int main(int argc, char **argv) {
    if (argc != 4 || (strcmp(argv[1], "-c") != 0 && strcmp(argv[1], "-d") != 0)) {
        fprintf(stderr, "Usage: %s -c|-d <input> <output>\n", argv[0]);
        return 1;
    }

    int is_compress = (strcmp(argv[1], "-c") == 0);
    size_t inLen;
    uint8_t *in = read_file(argv[2], &inLen);
    if (!in) { fprintf(stderr, "Failed to read %s\n", argv[2]); return 1; }

    size_t outCap = is_compress ? mylz4_compress_bound(inLen) : 64 * 1024 * 1024;
    uint8_t *out = malloc(outCap);
    
    size_t outLen = 0;
    if (is_compress) {
        outLen = mylz4_compress(in, inLen, out, outCap);
    } else {
        outLen = mylz4_decompress(in, inLen, out, outCap);
    }

    if (outLen == 0) {
        fprintf(stderr, "Operation failed! (Buffer too small or corrupt data)\n");
        free(in); free(out); return 1;
    }

    FILE *f = fopen(argv[3], "wb");
    if (!f) { perror("fopen"); free(in); free(out); return 1; }
    fwrite(out, 1, outLen, f);
    fclose(f);

    printf("Success: %zu bytes -> %zu bytes (Ratio: %.2f)\n", inLen, outLen, (double)inLen / outLen);
    free(in); free(out);
    return 0;
}
