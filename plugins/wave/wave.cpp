#include "wave.hpp"

Wave::Wave(std::string file_path) : FileFormat {file_path} {
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