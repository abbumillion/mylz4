#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <lz4.h>

int main(int argc, char **argv) {
    if (argc != 3) return 1;
    FILE *f = fopen(argv[1], "rb");
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    char *comp = malloc(sz);
    fread(comp, 1, sz, f); fclose(f);

    // Allocate a big buffer for the output
    int outCap = 1024 * 1024;
    char *out = malloc(outCap);

    int dec_len = LZ4_decompress_safe(comp, out, sz, outCap);
    if (dec_len < 0) {
        printf("FAIL: Real LZ4 could not decode mylz4 output! Error: %d\n", dec_len);
        return 1;
    }

    FILE *o = fopen(argv[2], "wb");
    fwrite(out, 1, dec_len, o);
    fclose(o);
    printf("Real LZ4 successfully decoded mylz4 output (%d bytes).\n", dec_len);
    return 0;
}
