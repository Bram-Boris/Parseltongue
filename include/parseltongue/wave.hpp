#ifndef FILE_WAVE_HPP
#define FILE_WAVE_HPP

#include "parseltongue/file_format.hpp"
#include "parseltongue/utf_8_validator.hpp"
#include "parseltongue/print_helper.hpp"

class Wave : public FileFormat {
public:
    Wave(std::string file_path) : FileFormat {file_path} {
        header_riff = read_array<char, 4>(0, true).get();
        header_file_size = read<uint32_t>(4);
        header_wave = read_array<char, 4>(8, true).get();

        header_fmt_start = read_array<char, 4>(12, true).get();
        header_fmt_length = read<uint32_t>(16);
        header_file_encoding_tag = read<uint16_t>(20);
        header_channels = read<uint16_t>(22);
        header_sample_rate = read<uint32_t>(24);
        header_byte_rate = read<uint32_t>(28);
        header_block_align = read<uint16_t>(32);
        header_bits_per_sample = read<uint16_t>(34);

        header_data_start = read_array<char, 4>(36, true).get();
        header_data_length = read<uint32_t>(40);
    }
    std::vector<std::string> get_file_extensions() const override {
        return std::vector<std::string> {
            "wav",
            "wave"
        };
    }
    std::string get_file_format_name() const override { return "WAVE"; }
    void speak_parseltongue(std::string) override {
    }
    std::vector<std::string> read_parseltongue() override {
        // Data starts at 44.
        int data_offset = 44;
        int sample_size = header_channels * header_bits_per_sample / 8;
        bool* buffer = new bool[100000];
        char* chars = new char[100000];
        int tries = 0;
        int bits_added = 0;
        for(uint32_t sample_offset = 0; sample_offset < header_data_length; sample_offset += sample_size) {
            char sample_byte = read<char>(data_offset + sample_offset);
            //std::cout << data_offset + sample_offset << std::endl;
            //print_helper::print_binary_char(sample_byte);
            bool bit = (sample_byte >> 0) & 1;
            //std::cout << bit << std::endl;
            buffer[bits_added] = bit;
            ++bits_added;
            if(bits_added >= 8 && bits_added % 8 == 0) {
                //std::cout << first_byte << std::endl;
                //std::cout << buffer << std::endl;
                std::bitset<8> bs;
                //std::cout << bits_added << std::endl;
                for (int i = bits_added - 8, j = 7; i < bits_added; i++, j--) {
                    bs.set(j, buffer[i]);
                }
                chars[bits_added / 8 - 1] = (char)bs.to_ulong();
                //std::cout << "byte found" << std::endl;
                //print_helper::print_binary_char(chars[bits_added / 8 - 1]);
                //print_helper::print_binary_char(*first_byte);

                if(chars[bits_added / 8 - 1] == '\0') {
                    //std::string str { reinterpret_cast<char*>(buffer) };
                    std::string str { chars };
                    // Also check if the str isn't only a NULL byte
                    if (utf8::validate(str) && !str.empty()) {
                        std::cout << str << '\n';
                        std::cout << "-------------------------------------\n";
                        bits_added = 0;
                        buffer = new bool[100000];
                        chars = new char[100000];
                        break;
                    }
                    else {
                        bits_added = 0;
                        buffer = new bool[10000];
                        chars = new char[100000];
                    }
                }
            }
        }
        return std::vector<std::string>();
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