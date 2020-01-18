#ifndef FILE_TIFF_HPP
#define FILE_TIFF_HPP

#include <bitset>
#include <endian.h>
#include <string>

#include "parseltongue/file_format.hpp"
#include "parseltongue/utf_8_validator.hpp"
#include "parseltongue/print_helper.hpp"

struct Tiff_header {
    enum endianness endianness_;
    uint16_t version_number;
    uint32_t first_directory_offset;
};

struct Tiff_image_data {
    uint32_t image_length;
    uint32_t rows_per_strip;
    uint32_t strips_per_image;
    uint16_t bit_per_sample_R;
    uint16_t bit_per_sample_G;
    uint16_t bit_per_sample_B;
    uint16_t samples_per_pixel;
    std::vector<uint32_t> strip_byte_counts;
    std::vector<uint32_t> strip_offsets;
};

class Tiff : public FileFormat {
public:
    Tiff(std::string file_path);
    ~Tiff() override = default;

    virtual enum endianness get_endianness() const override {
        return tiff_header.endianness_;
    }

    void speak_parseltongue(std::string message) override;
    void read_parseltongue() override;

    void print_header() const override {}

private:
    uint32_t sample_length() const {
        return (tiff_data.bit_per_sample_R + tiff_data.bit_per_sample_G + tiff_data.bit_per_sample_B) / 8;
    }
    uint32_t blue_channel_offset() const {
        return (tiff_data.bit_per_sample_R + tiff_data.bit_per_sample_G) / 8;
    }

    Tiff_header tiff_header;
    Tiff_image_data tiff_data;
};

#endif // FILE_TIFF_HPP