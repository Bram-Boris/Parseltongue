#include "aiff.hpp"
#include <cstring>
#include "parseltongue/file_format.hpp"
#include "parseltongue/exceptions/file_format_exception.hpp"

Aiff::Aiff(std::string file_path) : FileFormat{file_path} {
    aiff_header.aiff_start = std::string(read_array<char, 4>(0, true).get());
    aiff_header.file_length = read<uint32_t>(4) + 8;
    aiff_header.aiff_start = std::string(read_array<char, 4>(8, true).get());

    uint32_t pos = 12;
    bool COMM_found = false;
    bool SSND_found = false;
    while(pos < aiff_header.file_length) {
        std::string chunk_type { read_array<char, 4>(pos, true).get() }; 
        int32_t chunk_length { read<int32_t>(pos + 4) }; 
        if (chunk_type == "COMM") {
            COMM_found = true;
            COMM_header.chunk_offset = pos;
            COMM_header.start = chunk_type;
            COMM_header.chunk_length = chunk_length;
            COMM_header.channels = read<int16_t>(pos + 8);
            COMM_header.frames = read<uint32_t>(pos + 10);
            COMM_header.bits_per_sample = read<int16_t>(pos + 14);
            COMM_header.sample_rate = std::move(read_array<char, 10>(pos + 16));
        }
        if (chunk_type == "SSND") {
            SSND_found = true;
            SSND_header.chunk_offset = pos;
            SSND_header.start = chunk_type;
            SSND_header.chunk_length = chunk_length;
            SSND_header.data_offset = read<uint32_t>(pos + 8);
            SSND_header.block_size = read<uint32_t>(pos + 12);
        }

        pos += chunk_length + 8;
    }
    if (!COMM_found || !SSND_found)
        throw FileFormatException("AIFF could not be read.");
}
void Aiff::speak_parseltongue(std::string message) {
    uint32_t sample_offset = 0;
    for (int i = 0; i < message.length() + 1; i++) {
        char c = message.c_str()[i];
        std::bitset<8> bits(c);
        for (int b = 7; b >= 0; b--) {
            unsigned char sample_byte = read<char>(data_offset() + sample_offset + channel_sub_sample_length() - 1);
            bool bit = bits[b];
            if (!bit)
                sample_byte &= ~1;
            else
                sample_byte |= 1;
            write<unsigned char>(sample_byte, data_offset() + sample_offset + channel_sub_sample_length() - 1);
            sample_offset += channel_sub_sample_length();
        }
    }
}
std::vector<std::string> Aiff::read_parseltongue() {
    std::vector<std::string> messages;
    std::bitset<100000> buffer;
    std::string message;
    int bits_added = 0;
    for(uint32_t sample_offset = 0; sample_offset < SSND_header.chunk_length; sample_offset += channel_sub_sample_length()) {
        const unsigned char sample_byte = read<char>(data_offset() + sample_offset + channel_sub_sample_length() - 1);
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
                    messages.push_back(message);
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
    return messages;
}
