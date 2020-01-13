#include <iostream>
#include <unicode/ucnv.h>
#include <fstream>
#include <bitset>

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

void print_binary_char(char& c) {
    int i;
    for (i = 0; i < 8; i++) {
        printf("%d", !!((c << i) & 0x80));
    }
    printf("\n");
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

        stream.seekg(20);
        uint16_t encoding_tag;
        stream.read(reinterpret_cast<char*>(&encoding_tag), sizeof(encoding_tag));

        stream.seekg(22);
        uint16_t channels;
        stream.read(reinterpret_cast<char*>(&channels), sizeof(channels));

        stream.seekg(24);
        uint32_t sample_rate;
        stream.read(reinterpret_cast<char*>(&sample_rate), sizeof(sample_rate));

        stream.seekg(28);
        uint32_t byte_rate;
        stream.read(reinterpret_cast<char*>(&byte_rate), sizeof(byte_rate));

        stream.seekg(32);
        uint16_t block_align;
        stream.read(reinterpret_cast<char*>(&block_align), sizeof(block_align));

        stream.seekg(34);
        uint16_t bits_per_sample;
        stream.read(reinterpret_cast<char*>(&bits_per_sample), sizeof(bits_per_sample));

        stream.seekg(36);
        char* data_start_text = new char[5];
        data_start_text[4] = '\0';
        stream.read(data_start_text, sizeof(char) * 4);

        stream.seekg(40);
        uint32_t sample_data_size;
        stream.read(reinterpret_cast<char*>(&sample_data_size), sizeof(sample_data_size));

        int data_offset = 44;
        int sample_size = channels * bits_per_sample / 8;
        std::bitset<1000> collected_bits {};
        //std::cout << ((0b0001 >> 0) & 1) << std::endl;
        int tries = 0;
        int bits_added = 0;
        char f = 'Z' | 1;
        print_binary_char(f);
        char z = 'Z';
        print_binary_char(z);
        for(uint32_t sample_offset = 0; sample_offset < sample_data_size; sample_offset += sample_size) {
            stream.seekg(data_offset + sample_offset);
            char byte = stream.peek();
            collected_bits >>= 1;
            bool bit = (byte >> 0) & 1;
            collected_bits.set(999, bit);
            //collected_chars |= (bool)((byte >> 0) & 1);
            //std::cout << ((collected_chars[999] | collected_chars[998]) == 0) << std::endl;
            ++bits_added;
            if(bits_added >= 8 && bits_added % 8 == 0 && (collected_bits[992] | collected_bits[999] | collected_bits[998] | collected_bits[997] | collected_bits[996] | collected_bits[995]
                | collected_bits[994] | collected_bits[993]) == 0) {
                //UErrorCode status = U_ZERO_ERROR;
                //UConverter* const cnv = ucnv_open("utf-8", &status);
                //assert(U_SUCCESS(status));
                //int targetLimit = 2 * str.size();
                //UChar *target = new UChar[targetLimit];
                //ucnv_toUChars(cnv, target, targetLimit, collected_chars.c_str(), -1, &status);
                int bytes_added = bits_added / 8;
                char* buffer = new char[1000];
                for (int byte_i = 0; byte_i < bytes_added; byte_i++) {
                    int bits_byte_index = 991 - byte_i * 8;
                    char& c = buffer[byte_i];
                    for (int y = 0; y < 8; y++) {
                        c |= collected_bits[bits_byte_index + y];
                        c <<= 1;
                    }
                }
                std::string str { buffer };

                if (utf8_check_is_valid(str)) {
                    std::cout << str << "\n";
                    std::cout << "-------------------------------------\n";
                    bits_added = 0;
                    collected_bits.reset();
                }
                else {
                    //std::cout << "Found a null byte, but it wasn't valid UTF-*. :O" << std::endl;
                    bits_added = 0;
                    collected_bits.reset();
                }
            }
        }
        std::cout << std::flush;

        std::cout << "offset: 0 - " << riff << std::endl;
        std::cout << "offset: 4 - " << file_length << std::endl;
        std::cout << "offset: 8 - " << wave << std::endl;

        std::cout << "offset: 12 - " << fmt_start_text << std::endl;
        std::cout << "offset: 16 - " << fmt_length << std::endl;
        std::cout << "offset: 20 - " << encoding_tag << std::endl;
        std::cout << "offset: 22 - " << channels << std::endl;
        std::cout << "offset: 24 - " << sample_rate << std::endl;
        std::cout << "offset: 28 - " << byte_rate << std::endl;
        std::cout << "offset: 32 - " << block_align << std::endl;
        std::cout << "offset: 34 - " << bits_per_sample << std::endl;

        std::cout << "offset: 36 - " << data_start_text << std::endl;
        std::cout << "offset: 40 - " << sample_data_size << std::endl;

        delete[] riff;
        delete[] wave;
    }

    stream.close();
}
