#ifndef FILE_PNG_HPP
#define FILE_PNG_HPP

#include <string>
#include <vector>

#include "parseltongue/file_format.hpp"
#include "parseltongue/endian.hpp"

struct PNG_Chunk {
    uint32_t chunk_length;
    uint32_t crc;
};

struct PNG_IHDR : PNG_Chunk {
    uint32_t width;
    uint32_t height;
    uint8_t bit_depth;
    uint8_t color_type;
    uint8_t compression_type;
    uint8_t filter_method;
    uint8_t interlace_method;

    static const uint32_t type = 0x49484452;
};

struct PNG_PLTE : PNG_Chunk {
    static const uint32_t type = 80768469;
};

struct PNG_IDAT : PNG_Chunk {
    uint8_t zlib_flags;
    uint8_t additional_flags;
    uint32_t check_value;
    uint32_t data_offset;
    uint32_t data_length;
    static const uint32_t type = 0x49444154;
};

struct PNG_IEND : PNG_Chunk {
    static const uint32_t type = 0x49454E44;
};

class PNG : public FileFormat {
public:
    PNG(std::string file_path);
    ~PNG() override = default;

    virtual enum endianness endianness() const override {
        return endianness::big;
    }

    void speak_parseltongue(std::string message) override;
    std::vector<std::string> read_parseltongue() override;

    void print_header() const override {
    }

private:
    std::vector<PNG_IDAT> PNG_idats;
    PNG_IEND PNG_iend;
    PNG_IHDR PNG_ihdr;

    static const uint64_t start_png = 0x89504e470d0a1a0a; 
};

#endif // FILE_PNG_HPP