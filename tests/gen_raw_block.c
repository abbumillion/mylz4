#include <stdio.h>
#include <stdlib.h>
#include <lz4.h>

int main(int argc, char **argv) {
    if (argc != 3) { fprintf(stderr, "Usage: %s in out\n", argv[0]); return 1; }
    
    FILE *f = fopen(argv[1], "rb");
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    char *in = malloc(sz);
    fread(in, 1, sz, f); fclose(f);

    int max_out = LZ4_compressBound(sz);
    char *out = malloc(max_out);
    int out_len = LZ4_compress_default(in, out, sz, max_out);

    FILE *o = fopen(argv[2], "wb");
    fwrite(out, 1, out_len, o);
    fclose(o);
    
    printf("Compressed %ld bytes -> %d bytes (Raw Block)\n", sz, out_len);
    return 0;
}
