#ifndef FILE_FILE_FORMAT_HPP
#define FILE_FILE_FORMAT_HPP

#include <string>
#include <vector>
#include <fstream>
#include <memory>
#include <iostream>
#include <exception>
#include <optional>

#include "parseltongue/endian.hpp"
#include "parseltongue/print_helper.hpp"

class FileFormat {
public:
    FileFormat(std::string file_path) : byte_stream{file_path, std::ios::binary | std::ios::in | std::ios::out} {
        if (!byte_stream.is_open()) {
            byte_stream.close();
            throw std::runtime_error("File could not be found: " + file_path);
        }
    }
    virtual ~FileFormat() {
        byte_stream.close();
    }

    virtual endianness endianness() const = 0;
    virtual void print_header() const = 0;
    virtual void speak_parseltongue(std::string) = 0;
    virtual void read_parseltongue() = 0;
protected:
    // When not giving the position, the position in the stream will change!
    template<typename F>
    void read_out(F& out, std::optional<std::size_t> pos = std::nullopt) {
        std::size_t previous_pos = byte_stream.tellg();
        if (pos)
            byte_stream.seekg(*pos);
        byte_stream.read(reinterpret_cast<char*>(&out), sizeof(F));
        if (sizeof(F) > 1 && endianness() == endianness::big)
            convert_endianness(&out);
        if (pos)
            byte_stream.seekg(previous_pos);
    }
    template<typename F>
    F read(std::optional<std::size_t> pos = std::nullopt) {
        F out {};
        read_out(out, pos);
        return out;
    }
    template<typename F>
    void read_array_out(F* out, int N, std::optional<std::size_t> pos = std::nullopt, bool add_null_byte = false) {
        std::size_t n = add_null_byte ? N + 1 : N;
        std::size_t previous_pos = byte_stream.tellg();
        if (pos)
            byte_stream.seekg(*pos);
        byte_stream.read(reinterpret_cast<char*>(out), sizeof(F) * N);
        if (add_null_byte)
            out[n - 1] = '\0';
        if (sizeof(F) > 1 && endianness() == endianness::big) {
            for (int i = 0; i < N; i++) {
                convert_endianness(&out[i]);
            }
        }
        // Go back to previous position
        if (pos)
            byte_stream.seekg(previous_pos);
    }
    template<typename F>
    std::unique_ptr<F[]> read_array(int N, std::optional<std::size_t> pos = std::nullopt, bool add_null_byte = false) {
        std::size_t n = add_null_byte ? N + 1 : N;
        std::unique_ptr<F[]> out = std::unique_ptr<F[]>(new F[n]);
        read_array_out<F>(out.get(), N, pos, add_null_byte);
        return out;
    }
    template<typename F>
    void write(F& f, std::optional<std::size_t> pos = std::nullopt) {
        std::size_t previous_pos = byte_stream.tellg();
        if (pos)
            byte_stream.seekg(*pos);

        // write
        byte_stream.write(reinterpret_cast<char*>(&f), sizeof(F));
    
        // Go back to previous position
        if (pos)
            byte_stream.seekg(previous_pos);
    }

    std::fstream byte_stream;
};

#endif // FILE_FILE_FORMAT_HPP
