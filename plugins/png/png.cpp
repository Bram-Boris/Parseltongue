#include "png.hpp"
#include <cassert>
#include <zlib.h>
#include <stdexcept>

PNG::PNG(std::string file_path) : FileFormat {file_path} {
    if (read<uint64_t>(0) != start_png) {
        throw std::runtime_error("This is not a PNG file");
    }
    byte_stream.seekg(0, std::ios::end);
    auto file_length = byte_stream.tellg();
    byte_stream.seekg(0);
    uint32_t current_chunk_offset = 8;
    bool first_idat_found = false;
    while (current_chunk_offset < file_length) {
        uint32_t data_length = read<uint32_t>(current_chunk_offset);
        std::cout << "Chunk data length: " << data_length << std::endl;
        uint32_t type = read<uint32_t>(current_chunk_offset + 4);
        if (type == PNG_IHDR::type) {
            std::cout << "IHDR" << std::endl;
            PNG_IHDR ihdr;
            ihdr.width = read<uint32_t>(current_chunk_offset + 8);
            ihdr.height = read<uint32_t>(current_chunk_offset + 12);
            ihdr.bit_depth = read<uint8_t>(current_chunk_offset + 16);
            ihdr.color_type = read<uint8_t>(current_chunk_offset + 17);
            ihdr.compression_type = read<uint8_t>(current_chunk_offset + 18);
            ihdr.filter_method = read<uint8_t>(current_chunk_offset + 19);
            ihdr.interlace_method = read<uint8_t>(current_chunk_offset + 20);

            if (ihdr.compression_type != 0)
                throw std::runtime_error("This PNG uses an undefined compression method");

            std::cout << "IHDR width: " << ihdr.width << std::endl;
            std::cout << "IHDR height: " << ihdr.height << std::endl;

            std::cout << "IHDR Compression method: ";
            print_helper::print_binary(ihdr.compression_type);

            
        } else if (type == PNG_IDAT::type) {
            std::cout << "IDAT" << std::endl;
            PNG_IDAT idat;
            if(!first_idat_found) {
                PNG_idat_info.zlib_flags = read<uint8_t>(current_chunk_offset + 8);
                std::bitset<8> zlib_flags_bs { PNG_idat_info.zlib_flags };
                if (zlib_flags_bs[0] != 0 || zlib_flags_bs[1] != 0 || zlib_flags_bs[2] != 0 || zlib_flags_bs[3] != 1)
                    throw std::runtime_error("This PNG isn't compressed with deflate compression!");
                print_helper::print_binary(PNG_idat_info.zlib_flags);
                PNG_idat_info.additional_flags = read<uint8_t>(current_chunk_offset + 9);
                std::bitset<8> additional_flags_bs { PNG_idat_info.additional_flags };
                if (additional_flags_bs[5] != 0)
                    throw std::runtime_error("This PNG has a FDICT value of 1, which is not supported by the PNG standard");
                print_helper::print_binary(PNG_idat_info.additional_flags);
                idat.data_offset = current_chunk_offset + 10;
                idat.data_length = data_length - 6;
                first_idat_found = true;
            } else {
                idat.data_offset = current_chunk_offset + 8;
                idat.data_length = data_length;
            }
            idat.crc = read<uint32_t>(current_chunk_offset + data_length + 8);
            std::cout << "crc: ";
            print_helper::print_binary(idat.crc);
            PNG_idats.push_back(idat);
        } else if (type == PNG_IEND::type) {
            std::cout << "IEND" << std::endl;
        }
        current_chunk_offset += data_length + 12;
    }
    auto& last_idat = PNG_idats.back();
    PNG_idat_info.check_value = read<uint32_t>(last_idat.data_offset + last_idat.data_length);
}

void PNG::speak_parseltongue(std::string message) {}


#define CHUNK 16384
std::vector<unsigned char> PNG::inflate2(unsigned char* in, size_t in_length) {
    int ret;
    unsigned have;
    z_stream strm;
    unsigned char out[1000000];
    std::vector<unsigned char> out_data;
    /* allocate inflate state */
    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;
    strm.avail_in = 0;
    strm.next_in = Z_NULL;
    ret = inflateInit(&strm);
    if (ret != Z_OK)
        throw std::runtime_error("inflate_init error");

    /* decompress until deflate stream ends or end of file */
    size_t current_in = 0;
    do {
        strm.avail_in = CHUNK;
        if (strm.avail_in == 0)
            break;
        strm.next_in = in + current_in;

        /* run inflate() on input until output buffer not full */
        do {
            strm.avail_out = CHUNK;
            strm.next_out = out;
            ret = inflate(&strm, Z_NO_FLUSH);
            assert(ret != Z_STREAM_ERROR);  /* state not clobbered */
            switch (ret) {
            case Z_NEED_DICT:
                ret = Z_DATA_ERROR;     /* and fall through */
            case Z_DATA_ERROR:
            case Z_MEM_ERROR:
                (void)inflateEnd(&strm);
                throw std::runtime_error("mem error error");
            }
            have = CHUNK - strm.avail_out;
            for (size_t i = 0; i < have; i++) {
                out_data.push_back(out[i]);
            }
        } while (strm.avail_out == 0);

        current_in += CHUNK;

        /* done when inflate() says it's done */
    } while (ret != Z_STREAM_END && current_in < in_length);


    /* clean up and return */
    (void)inflateEnd(&strm);
    if (ret != Z_STREAM_END)
        throw std::runtime_error("not at end error!");
    return out_data;
}

void PNG::read_parseltongue() {
    std::bitset<100000> buffer;
    std::string message;
    int bits_added = 0;
    size_t buffer_length = 0;
    for(PNG_IDAT idat : PNG_idats) {
        buffer_length += idat.data_length;
    }

    unsigned char data[buffer_length]; 
    int current_data_offset = 0;
    for(PNG_IDAT idat : PNG_idats) {
        read_array_out<unsigned char>(&data[current_data_offset], idat.data_length);
        current_data_offset += idat.data_length;
    }

    //auto out_data = inflate2(data, buffer_length);
    std::cout << "Als je dit leest dan werkt PNG support niet :'( " << std::endl;
}