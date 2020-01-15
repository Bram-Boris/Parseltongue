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
    void speak_parseltongue(std::string) override {
    }
    std::vector<std::string> read_parseltongue() override {
        std::vector<std::string> messages;
        const int data_offset = 44;
        const int sample_size = header_bits_per_sample / 8;
        // Maybe move the 100000 to a const and enforce it in the inputs
        std::bitset<100000> buffer;
        std::string message;
        int bits_added = 0;
        for(uint32_t sample_offset = 0; sample_offset < header_data_length; sample_offset += sample_size) {
            const char sample_byte = read<char>(data_offset + sample_offset);
            const bool bit = (sample_byte >> 0) & 1;
            buffer[bits_added] = bit;
            ++bits_added;
            if(bits_added >= 8 && bits_added % 8 == 0) {
                std::bitset<8> char_bits;
                for (int i = bits_added - 8, j = 7; i < bits_added; i++, j--) {
                    char_bits.set(j, buffer[i]);
                }
                char c = static_cast<char>(char_bits.to_ulong());
                if(c != '\0') {
                    message.push_back(c);
                } else {
                    if (utf8::validate(message) && !message.empty())
                        messages.push_back(message);

                    bits_added = 0;
                    message = std::string {};
                    // this really do anything, because we can just overwrite the bits
                    //buffer.reset();
                }
            }
        }
        return messages;
    }

    void print_header() const override {
        std::cout << "offset: 0 - " << header_riff << std::endl;
        std::cout << "offset: 4 - " << header_file_size << std::endl;
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
};

#endif // FILE_WAVE_HPP
