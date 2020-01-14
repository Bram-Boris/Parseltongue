#ifndef FILE_PRINT_HELPER_HPP
#define FILE_PRINT_HELPER_HPP

#include <cstdio>

class print_helper {
public:
static void print_binary_char(char& c) {
    int i;
    for (i = 0; i < 8; i++) {
        printf("%d", !!((c << i) & 0x80));
    }
    printf("\n");
}
};

#endif // FILE_PRINT_HELPER_HPP