#ifndef FILE_WAVE_HPP
#define FILE_WAVE_HPP

#include <bitset>

#include "parseltongue/file_format.hpp"
#include "parseltongue/utf_8_validator.hpp"
#include "parseltongue/print_helper.hpp"

class Wave : public FileFormat {
public:
    Wave(std::string file_path);
    ~Wave() override = default;

    virtual enum endianness endianness() const override {
        return endianness::little;
    }
    void speak_parseltongue(std::string message) override;
    void read_parseltongue() override;

    void print_header() const override {
        std::cout << "offset: 0 - " << header_riff << std::endl;
        std::cout << "offset: 4 - " << header_file_size << std::endl;
        std::cout << "offset: 8 - " << header_wave << std::endl;
        std::cout << "offset: 12 - " << header_fmt_start<< std::endl;
        std::cout << "offset: 16 - " << header_fmt_length << std::endl;
        std::cout << "offset: 20 - " << header_file_encoding_tag << std::endl;
        std::cout << "offset: 22 - " << header_channels << std::endl;
        std::cout << "offset: 24 - " << header_sample_rate << std::endl;
        std::cout << "offset: 28 - " << header_byte_rate << std::endl;
        std::cout << "offset: 32 - " << header_block_align << std::endl;
        std::cout << "offset: 34 - " << header_bits_per_sample << std::endl;

        std::cout << "offset: 36 - " << header_data_start<< std::endl;
        std::cout << "offset: 40 - " << header_data_length << std::endl;
    }
private:
    static const int DATA_OFFSET = 44;

    // Header
    std::string header_riff;
    uint32_t header_file_size;
    std::string header_wave;

    std::string header_fmt_start;
    uint32_t header_fmt_length;
    uint16_t header_file_encoding_tag;
    uint16_t header_channels;
    uint32_t header_sample_rate;
    uint32_t header_byte_rate;
    uint16_t header_block_align;
    uint16_t header_bits_per_sample;

    std::string header_data_start;
    uint32_t header_data_length;

    int sample_length() const {
        return header_channels * header_bits_per_sample / 8;
    }
    int channel_sub_sample_length() const {
        return header_bits_per_sample / 8;
    }
};

#endif // FILE_WAVE_HPP
