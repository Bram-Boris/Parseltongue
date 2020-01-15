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


class Tiff : public FileFormat {
public:
    Tiff(std::string file_path);
    ~Tiff() override = default;

    virtual enum endianness endianness() const override {
        return tiff_header.endianness_;
    }

    void speak_parseltongue(std::string message) override;
    std::vector<std::string> read_parseltongue() override;

    void print_header() const override {
    }
private:
    Tiff_header tiff_header;
};

#endif // FILE_TIFF_HPP