#ifndef FILE_ENDIAN_HPP
#define FILE_ENDIAN_HPP

#include <algorithm>
#include <endian.h>

enum class endianness {
    little,
    big
};
template <class T>
void convert_endianness(T* objp) {
    unsigned char *memp = reinterpret_cast<unsigned char*>(objp);
    std::reverse(memp, memp + sizeof(T));
}

#endif // FILE_ENDIAN_HPP