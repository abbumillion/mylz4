#include <stdio.h>
#include <stdlib.h>
#include <lz4.h>

int main(int argc, char **argv) {
    if (argc != 3) return 1;
    FILE *f = fopen(argv[1], "rb");
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    char *comp = malloc(sz);
    fread(comp, 1, sz, f); fclose(f);

    int outCap = 64 * 1024 * 1024;
    char *out = malloc(outCap);

    int dec_len = LZ4_decompress_safe(comp, out, sz, outCap);
    if (dec_len < 0) {
        fprintf(stderr, "Real LZ4 failed to decode! Error: %d\n", dec_len);
        return 1;
    }

    FILE *o = fopen(argv[2], "wb");
    fwrite(out, 1, dec_len, o);
    fclose(o);
    return 0;
}
