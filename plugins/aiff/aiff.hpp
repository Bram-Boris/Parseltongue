#ifndef FILE_AIFF_HPP
#define FILE_AIFF_HPP

#include <bitset>
#include <endian.h>
#include <string>

#include "parseltongue/file_format.hpp"
#include "parseltongue/utf_8_validator.hpp"
#include "parseltongue/print_helper.hpp"

struct Aiff_header {
    std::string start;
    uint32_t file_length;
    std::string aiff_start;
};


struct Aiff_COMM_header {
    uint32_t chunk_offset;

    std::string start;
    uint32_t chunk_length;
    uint16_t channels;
    uint32_t frames;
    uint16_t bits_per_sample;
    std::unique_ptr<char[]> sample_rate; // just don't read this shit, you can reinterpret it as a 80bit float if you dare.
};
struct Aiff_SSND_header {
    uint32_t chunk_offset;

    std::string start;
    uint32_t chunk_length;
    uint32_t data_offset;
    uint32_t block_size;
    //16     (n)bytes  Comment
    //16+(n) (s)bytes  <Sample data>

};

class Aiff : public FileFormat {
public:
    Aiff(std::string file_path);
    ~Aiff() override = default;

    virtual enum endianness endianness() const override {
        return endianness::big;
    }

    void speak_parseltongue(std::string message) override;
    void read_parseltongue() override;

    void print_header() const override {
        std::cout << "COMM" << std::endl;
        std::cout << "chunk offset: " << COMM_header.chunk_offset << std::endl;
        std::cout << "start: " << COMM_header.start << std::endl;
        std::cout << "chunk_length: " << COMM_header.chunk_length << std::endl;
        std::cout << "channels: " << COMM_header.channels << std::endl;
        std::cout << "frames: " << COMM_header.frames << std::endl;
        std::cout << "bits_per_sample: " << COMM_header.bits_per_sample << std::endl;
        //std::cout << "sample_rate: " << COMM_header.sample_rate << std::endl;

        std::cout << "SSND" << std::endl;
        std::cout << "chunk offset: " << SSND_header.chunk_offset << std::endl;
        std::cout << "start: " << SSND_header.start << std::endl;
        std::cout << "chunk_length: " << SSND_header.chunk_length << std::endl;
        std::cout << "data_offset: " << SSND_header.data_offset << std::endl;
        std::cout << "block_size: " << SSND_header.block_size << std::endl;
    }
private:
    // Headers
    Aiff_header aiff_header;
    Aiff_COMM_header COMM_header;
    Aiff_SSND_header SSND_header;

    int sample_length() const {
        return COMM_header.channels * COMM_header.bits_per_sample / 8;
    }
    int channel_sub_sample_length() const {
        return COMM_header.bits_per_sample / 8;
    }
    uint32_t data_offset() const {
        return SSND_header.chunk_offset + 16 + SSND_header.data_offset;
    }
};

#endif // FILE_aiff_HPP