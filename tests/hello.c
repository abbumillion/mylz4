#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <lz4.h>  // The REAL LZ4 library

int main() {
    // 1. The raw LZ4 block we hand-encoded
    // Token: 0xB0 (11 literals, 0 match length)
    // Literals: "hello world" (11 bytes)
    const uint8_t compressed[] = {
        0xB0, 
        'h', 'e', 'l', 'l', 'o', ' ', 'w', 'o', 'r', 'l', 'd'
    };
    int compLen = sizeof(compressed);
    
    // 2. The output buffer (needs extra padding for LZ4's safe decoder)
    char output[32];
    int expectedLen = 11; // "hello world" is 11 bytes
    
    printf("Compressed data length: %d bytes\n", compLen);
    
    // 3. Decode it using the REAL LZ4 library
    int decodedLen = LZ4_decompress_safe((const char*)compressed, output, compLen, sizeof(output));
    
    if (decodedLen != expectedLen) {
        printf("FAILED: Expected %d bytes, got %d\n", expectedLen, decodedLen);
        return 1;
    }
    
    output[decodedLen] = '\0'; // Null-terminate for printing
    printf("Decoded output: '%s'\n", output);
    
    if (strcmp(output, "hello world") == 0) {
        printf("SUCCESS! Hand-encoding is correct.\n");
    } else {
        printf("FAILED: Output does not match 'hello world'\n");
    }
    return 0;
}
