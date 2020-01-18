#include "wave.hpp"
#include <stdexcept>

Wave::Wave(std::string file_path) : FileFormat {file_path} {
    header_riff = read_array<char>(4, 0, true).get();
    header_file_size = read<uint32_t>(4);
    header_wave = read_array<char>(4, 8, true).get();

    header_fmt_start = read_array<char>(4, 12, true).get();
    header_fmt_length = read<uint32_t>(16);
    header_file_encoding_tag = read<uint16_t>(20);
    if (header_fmt_length != 16 || header_file_encoding_tag != 1)
        throw std::runtime_error("This file is not linear PCM, not supported");
    header_channels = read<uint16_t>(22);
    header_sample_rate = read<uint32_t>(24);
    header_byte_rate = read<uint32_t>(28);
    header_block_align = read<uint16_t>(32);
    header_bits_per_sample = read<uint16_t>(34);

    header_data_start = read_array<char>(4, 36, true).get();
    header_data_length = read<uint32_t>(40);
}

void Wave::speak_parseltongue(std::string message) {
    uint32_t sample_offset = 0;
    for (int i = 0; i < message.length() + 1; i++) {
        char c = message.c_str()[i];
        std::bitset<8> bits(c);
        for (int b = 7; b >= 0; b--) {
            unsigned char sample_byte = read<char>(DATA_OFFSET + sample_offset);
            bool bit = bits[b];
            if (!bit)
                sample_byte &= ~1;
            else
                sample_byte |= 1;
            write<unsigned char>(sample_byte, DATA_OFFSET + sample_offset);
            sample_offset += channel_sub_sample_length();
        }
    }
}
void Wave::read_parseltongue() {
    // Maybe move the 100000 to a const and enforce it in the inputs
    std::bitset<100000> buffer;
    std::string message;
    int bits_added = 0;
    for(uint32_t sample_offset = 0; sample_offset < header_data_length; sample_offset += channel_sub_sample_length()) {
        const unsigned char sample_byte = read<char>(DATA_OFFSET + sample_offset);
        const bool bit = sample_byte & 1;
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
                if (utf8::validate(message) && !message.empty()) {
                    std::cout << "A message has been found: " << std::endl;
                    std::cout << message << std::endl;
                }

                bits_added = 0;
                message = std::string {};
                // this really do anything, because we can just overwrite the bits
                //buffer.reset();
            }
        }
    }
}



