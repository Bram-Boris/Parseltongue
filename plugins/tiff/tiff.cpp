#include "tiff.hpp"
#include "parseltongue/exceptions/file_format_exception.hpp"

Tiff::Tiff(std::string file_path) : FileFormat{file_path} {
    std::string endianness_str { read_array<char, 2>(0, true).get() };
    if (endianness_str == "II")
        tiff_header.endianness_ = endianness::little;
    else if (endianness_str == "MM")
        tiff_header.endianness_ = endianness::big;
    else
        throw FileFormatException("Unsupported TIFF byte order");

    tiff_header.version_number = read<uint16_t>(2);
    std::cout << tiff_header.version_number << std::endl;
    tiff_header.first_directory_offset = read<uint32_t>(4);

    auto ifd_1_num_entries = read<uint16_t>(tiff_header.first_directory_offset);
    uint32_t field_offset = tiff_header.first_directory_offset + 2;
    std::cout << "NUM: " << ifd_1_num_entries << std::endl;
    for (int i = 0; i < ifd_1_num_entries; i++) {
        uint16_t tag = read<uint16_t>(field_offset);
        uint16_t type = read<uint16_t>(field_offset + 2);
        uint32_t length = read<uint32_t>(field_offset + 4);
        uint32_t offset = read<uint32_t>(field_offset + 8);
        field_offset += 12;
        std::cout << "tag: " << tag << std::endl;
        std::cout << "type: " << type << std::endl;
        std::cout << "length: " << length << std::endl;
        std::cout << "offset: " << offset << std::endl;
    }
    std::cout << ifd_1_num_entries << std::endl;
    std::cout << read<uint16_t>(tiff_header.first_directory_offset + 2) << std::endl;
    std::cout << read<uint16_t>(tiff_header.first_directory_offset + 2) << std::endl;
}

void Tiff::speak_parseltongue(std::string message) {

}

std::vector<std::string> Tiff::read_parseltongue() {
    return std::vector<std::string>();
}
