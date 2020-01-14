#ifndef FILE_PRINT_HELPER
#define FILE_PRINT_HELPER
#include <cstdio>

namespace print_helper {
    void print_binary_char(char& c) {
        int i;
        for (i = 0; i < 8; i++) {
            printf("%d", !!((c << i) & 0x80));
        }
        printf("\n");
    }
}

#endif // FILE_PRINT_HELPER