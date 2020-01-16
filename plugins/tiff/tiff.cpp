#include "tiff.hpp"
#include "parseltongue/exceptions/file_format_exception.hpp"
#include <cmath>

Tiff::Tiff(std::string file_path) : FileFormat{file_path} {
    std::string endianness_str { read_array<char, 2>(0, true).get() };
    if (endianness_str == "II")
        tiff_header.endianness_ = endianness::little;
    else if (endianness_str == "MM")
        tiff_header.endianness_ = endianness::big;
    else
        throw FileFormatException("Unsupported TIFF byte order");

    tiff_header.version_number = read<uint16_t>(2);
    std::cout << tiff_header.version_number << std::endl;
    tiff_header.first_directory_offset = read<uint32_t>(4);

    std::vector<bool> required_tags_found;
    uint32_t current_ifd = tiff_header.first_directory_offset;
    while (current_ifd != 0) {
        uint16_t ifd_num_entries = read<uint16_t>(tiff_header.first_directory_offset);
        uint32_t field_offset = tiff_header.first_directory_offset + 2;
        std::cout << "NUM: " << ifd_num_entries << std::endl;
        for (int i = 0; i < ifd_num_entries; i++) {
            uint16_t tag = read<uint16_t>(field_offset);
            uint16_t type = read<uint16_t>(field_offset + 2);
            uint32_t length = read<uint32_t>(field_offset + 4);
            uint32_t offset = read<uint32_t>(field_offset + 8);
            std::cout << "tag: " << tag << std::endl;
            std::cout << "type: " << type << std::endl;
            std::cout << "length: " << length << std::endl;
            std::cout << "offset: " << offset << std::endl;

            // Image length
            if (tag == 257) {
                tiff_data.image_length = offset;
                std::cout << "Image length: " << tiff_data.image_length << std::endl;
                required_tags_found.emplace_back(true);
            }
            // Bits per sample
            if (tag == 258) {
                tiff_data.bit_per_sample_R = read<uint16_t>(offset);
                tiff_data.bit_per_sample_G = read<uint16_t>(offset + 2);
                tiff_data.bit_per_sample_B = read<uint16_t>(offset + 4);
                if (tiff_data.bit_per_sample_R != 8 && tiff_data.bit_per_sample_G != 8 && tiff_data.bit_per_sample_B != 8)
                    throw FileFormatException("bit_per_sample value not supported, needs to be 8bit RGB");
                required_tags_found.emplace_back(true);
            }
            // Rows per strip
            if (tag == 278) {
                tiff_data.rows_per_strip = offset;
                std::cout << "Rows per strip: " << tiff_data.rows_per_strip << std::endl;
                if(tiff_data.rows_per_strip != 1)
                    throw FileFormatException("rows_per_strip value not supported, must be 1");
                required_tags_found.emplace_back(true);
            }
            // SamplesPerPixel
            if (tag == 277) {
                tiff_data.samples_per_pixel = read<uint16_t>(field_offset + 4);
                if (offset != 3)
                    throw FileFormatException("SamplesPerPixel value must be 3 for RGB.");
                required_tags_found.emplace_back(true);
            }
            // StripOffsets
            if (tag == 273) {
                if (type != 4)
                    throw FileFormatException("The field type of StripByteCounts must be uint32");
                for (int i = 0; i< length; i++) {
                    tiff_data.strip_offsets.push_back(read<uint32_t>(offset + i * 4));
                }
                required_tags_found.emplace_back(true);
            }
            // StripByteCounts
            if (tag == 279) {
                if (type != 4)
                    throw FileFormatException("The field type of StripByteCounts must be uint32");
                for (int i = 0; i< length; i++) {
                    tiff_data.strip_byte_counts.push_back(read<uint32_t>(offset + i * 4));
                }
                required_tags_found.emplace_back(true);
            }
            field_offset += 12;
        }
        current_ifd = read<uint32_t>(current_ifd + 2 + (ifd_num_entries * 12));
    }

    if (required_tags_found.size() != 6)
        throw FileFormatException("Not all the required tags have been found.");

    tiff_data.strips_per_image = floor((tiff_data.image_length * (tiff_data.rows_per_strip)) / tiff_data.rows_per_strip);
    std::cout << "Strips: " << tiff_data.strips_per_image << std::endl;
}

void Tiff::speak_parseltongue(std::string message) {
    std::size_t char_index = 0;
    for (int i = 0; i < tiff_data.strips_per_image; i++) {
        uint32_t strip_offset = tiff_data.strip_offsets[i];
        int b = 7;
        char c = message.c_str()[char_index];
        std::bitset<8> bits(c);
        for (uint32_t sample_offset = 0; sample_offset < tiff_data.strip_byte_counts[i]; sample_offset += sample_length()) {
            uint32_t offset = strip_offset + sample_offset + blue_channel_offset();
            unsigned char sample_byte = read<char>(offset);
            bool bit = bits[b];
            if (!bit)
                sample_byte &= ~1;
            else
                sample_byte |= 1;
            write<unsigned char>(sample_byte, offset);
            b--;
            if (b < 0) {
                char_index++;

                if (char_index > message.size())
                    return;

                b = 7;
                c = message.c_str()[char_index];
                bits = std::bitset<8>(c);
            }
        }
    }
}

std::vector<std::string> Tiff::read_parseltongue() {
    std::vector<std::string> messages;
    // Maybe move the 100000 to a const and enforce it in the inputs
    std::bitset<100000> buffer;
    std::string message;
    int bits_added = 0;
    for (int i = 0; i < tiff_data.strips_per_image; i++) {
        uint32_t strip_offset = tiff_data.strip_offsets[i];
        for (uint32_t sample_offset = 0; sample_offset < tiff_data.strip_byte_counts[i]; sample_offset += sample_length()) {
            const unsigned char sample_byte = read<char>(strip_offset + sample_offset + blue_channel_offset());
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
    }
    return messages;
}
