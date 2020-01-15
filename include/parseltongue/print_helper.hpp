#ifndef FILE_PRINT_HELPER_HPP
#define FILE_PRINT_HELPER_HPP

#include <iostream>
#include <bitset>

class print_helper {
public:
template<typename T>
static void print_binary(T& x) {
    std::bitset<sizeof(T) * 8> b { x };
    std::cout << b << std::endl;
}
};

#endif // FILE_PRINT_HELPER_HPP
