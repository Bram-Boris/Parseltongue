#include <iostream>
#include <unicode/ucnv.h>
#include <fstream>

namespace su {
    enum class byte_order {
        little_endian,
        big_endian
    };
    byte_order cur_byte_order() {
        const unsigned short val { *reinterpret_cast<const unsigned short *>("az") };
        return val == 0x617AU ? byte_order::big_endian : byte_order::little_endian;
    }
}

bool utf8_check_is_valid(const std::string& string)
{
    int c,i,ix,n,j;
    for (i=0, ix=string.length(); i < ix; i++)
    {
        c = (unsigned char) string[i];
        //if (c==0x09 || c==0x0a || c==0x0d || (0x20 <= c && c <= 0x7e) ) n = 0; // is_printable_ascii
        if (0x00 <= c && c <= 0x7f) n=0; // 0bbbbbbb
        else if ((c & 0xE0) == 0xC0) n=1; // 110bbbbb
        else if ( c==0xed && i<(ix-1) && ((unsigned char)string[i+1] & 0xa0)==0xa0) return false; //U+d800 to U+dfff
        else if ((c & 0xF0) == 0xE0) n=2; // 1110bbbb
        else if ((c & 0xF8) == 0xF0) n=3; // 11110bbb
        //else if (($c & 0xFC) == 0xF8) n=4; // 111110bb //byte 5, unnecessary in 4 byte UTF-8
        //else if (($c & 0xFE) == 0xFC) n=5; // 1111110b //byte 6, unnecessary in 4 byte UTF-8
        else return false;
        for (j=0; j<n && i<ix; j++) { // n bytes matching 10bbbbbb follow ?
            if ((++i == ix) || (( (unsigned char)string[i] & 0xC0) != 0x80))
                return false;
        }
    }
    return true;
}

int main() {
    std::cout << "Your processor has a " << (su::cur_byte_order() == su::byte_order::little_endian ? "little" : "big") << " endian architecture\n";

    std::string filename = "data/lounge1.wav";
    std::ifstream stream(filename, std::ios::binary);
    if (!stream.is_open()) {
        std::cout << "couldnt find file or something like that" << std::endl;
    } else {
        char* riff = new char[5];
        riff[4] = '\0';
        stream.read(riff, sizeof(char) * 4);

        int32_t file_length;
        stream.read(reinterpret_cast<char*>(&file_length), sizeof file_length);

        char* wave = new char[5];
        wave[4] = '\0';
        stream.read(wave, sizeof(char) * 4);

        char* fmt_start_text = new char[5];
        fmt_start_text[4] = '\0';
        stream.read(fmt_start_text, sizeof(char) * 4);

        uint32_t fmt_length;
        stream.read(reinterpret_cast<char*>(&fmt_length), sizeof(fmt_length));

        stream.seekg(36);

        char* data_start_text = new char[5];
        data_start_text[4] = '\0';
        stream.read(data_start_text, sizeof(char) * 4);

        stream.seekg(22);
        uint16_t channels;
        stream.read(reinterpret_cast<char*>(&channels), sizeof(channels));

        uint32_t sample_data_size;
        stream.read(reinterpret_cast<char*>(&sample_data_size), sizeof(sample_data_size));

        stream.seekg(32);
        uint16_t block_align;
        stream.read(reinterpret_cast<char*>(&block_align), sizeof(block_align));

        stream.seekg(34);
        uint16_t bits_per_sample;
        stream.read(reinterpret_cast<char*>(&bits_per_sample), sizeof(bits_per_sample));


        int data_offset = 44;
        int sample_size = channels * bits_per_sample / 8;
        std::string collected_chars {};
        for(int sample_offset = 0; sample_offset < sample_data_size; sample_offset += sample_size) {
            stream.seekg(data_offset + sample_offset);
            char c = stream.peek();
            collected_chars.push_back(c);
            if(c == '\0') {
                //UErrorCode status = U_ZERO_ERROR;
                //UConverter* const cnv = ucnv_open("utf-8", &status);
                //assert(U_SUCCESS(status));
                //int targetLimit = 2 * str.size();
                //UChar *target = new UChar[targetLimit];
                //ucnv_toUChars(cnv, target, targetLimit, collected_chars.c_str(), -1, &status);

                if (utf8_check_is_valid(collected_chars)) {
                    std::cout << collected_chars << std::endl;
                    std::cout << "-------------------------------------" << std::endl;
                    collected_chars.clear();
                }
                else {
                    //std::cout << "Found a null byte, but it wasn't valid UTF-*. :O" << std::endl;
                    collected_chars.clear();
                }
            }
        }

        std::cout << "offset: 0 - " << riff << std::endl;
        std::cout << "offset: 4 - " << file_length << std::endl;
        std::cout << "offset: 8 - " << wave << std::endl;

        std::cout << "offset: 12 - " << fmt_start_text << std::endl;
        std::cout << "offset: 16 - " << fmt_length << std::endl;
        //std::cout << "offset: 20 - " << encoding_tag << std::endl;
        std::cout << "offset: 22 - " << channels << std::endl;
        //std::cout << "offset: 24 - " << sample_rate << std::endl;
        //std::cout << "offset: 28 - " << bytes_per_second << std::endl;
        std::cout << "offset: 32 - " << block_align << std::endl;
        std::cout << "offset: 34 - " << bits_per_sample << std::endl;

        std::cout << "offset: 36 - " << data_start_text << std::endl;
        std::cout << "offset: 40 - " << sample_data_size << std::endl;

        delete[] riff;
        delete[] wave;
    }

    stream.close();
}
