#include "png.hpp"
#include "parseltongue/exceptions/file_format_exception.hpp"

PNG::PNG(std::string file_path) : FileFormat {file_path} {
    if (read<uint64_t>(0) != start_png) {
        throw FileFormatException("This is not a PNG file");
    }
    byte_stream.seekg(0, std::ios::end);
    auto file_length = byte_stream.tellg();
    byte_stream.seekg(0);
    uint32_t current_chunk_offset = 8;
    while (current_chunk_offset < file_length) {
        uint32_t data_length = read<uint32_t>(current_chunk_offset);
        uint32_t type = read<uint32_t>(current_chunk_offset + 4);
        if (type == PNG_IHDR::type) {
            std::cout << "IHDR" << std::endl;
        } else if (type == PNG_IDAT::type) {
            std::cout << "IDAT" << std::endl;
            PNG_IDAT idat;
            idat.zlib_flags = read<uint8_t>(current_chunk_offset + 8);
            idat.additional_flags = read<uint8_t>(current_chunk_offset + 9);
            idat.data_offset = current_chunk_offset + 10;
            idat.data_length = data_length - 6;
            idat.check_value = read<uint32_t>(idat.data_offset + idat.data_length);
            idat.crc = read<uint32_t>(current_chunk_offset + data_length);
            PNG_idats.push_back(idat);
        } else if (type == PNG_IEND::type) {
            std::cout << "IEND" << std::endl;
        }
        current_chunk_offset += data_length + 12;
    }
}

void PNG::speak_parseltongue(std::string message) {

}
std::vector<std::string> PNG::read_parseltongue() {
    return std::vector<std::string>();
}