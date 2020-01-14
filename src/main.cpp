#include <iostream>
#include <unicode/ucnv.h>
#include <fstream>
#include <bitset>
#include <getopt.h>
#include <dlfcn.h>
#include <filesystem>

#include "parseltongue/file_format.hpp"
#include "parseltongue/wave.hpp"

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

static struct option long_options[] = 
{
    {"file", required_argument, nullptr, 'f'},
    {"read", no_argument, nullptr, 'r'},
    {"write", required_argument, nullptr, 'w'},
};

enum class mode {
    READ,
    WRITE
};

std::pair<std::string, std::pair<mode, std::optional<std::string>>> process_input(int argc, char** argv) {
    std::optional<mode> m = std::nullopt;
    std::optional<std::string> f = std::nullopt;
    std::optional<std::string> wm = std::nullopt;
    while(true)
    {
        const auto opt = getopt_long(argc, argv, "f:rw:", long_options, nullptr);
        if(opt == -1)
            break;

        switch (opt) {
            case 'f':
                if(optarg) {
                    std::cout << "Opening file: " << optarg << std::endl;
                    f = std::string(optarg);
                } else {
                    std::cerr << "You need to supply a file name" << std::endl;
                    exit(1);
                }
                break;
            case 'r':
                std::cout << "Reading secrets from file" << std::endl;
                m = mode::READ;
                break;
            case 'w':
                if(optarg) {
                    std::cout << "Writing secrets to file: " << optarg << std::endl;
                    m = mode::WRITE;
                    wm = std::string(optarg);
                }
                else {
                    std::cerr << "You need to supply a secret when writing" << std::endl;
                    exit(1);
                }
               break;
        }
    }
    if (!m || !f) {
        std::cerr  << "You need to supply a file and a mode" << std::endl;
        exit(1);
    }
    return std::make_pair(*f, std::make_pair(*m, wm));
}

const char* plugin_ext = ".so";

int main(int argc, char** argv) {
    std::cout << "Your processor has a " << (su::cur_byte_order() == su::byte_order::little_endian ? "little" : "big") << " endian architecture\n";

    std::vector<std::string> plugin_paths;
    for(auto& p : std::filesystem::directory_iterator("./plugins")) {
        if (p.path().extension() == plugin_ext)
            plugin_paths.push_back(p.path().string());
    }

    std::vector<void*> plugins;
    for (auto& p : plugin_paths) {
        void* handle = dlopen(p.c_str(), RTLD_LAZY);
        char *errstr = dlerror();
        if (errstr)
            plugins.push_back(handle);
        else
            std::cout << "A dynamic linking error occurred: " << errstr << std::endl;
    }

    auto [file_path, mode_with_text] = process_input(argc, argv);
    auto [mode, write_text] = mode_with_text;
    // TODO: match extension to possible file types
    Wave w { file_path };
    if (mode == mode::READ) {
        w.print_header();
        w.read_parseltongue();
    } else if (mode == mode::WRITE) {
        std::cerr << "WWEE DOOO NOTTTT SUUOOPOPPORT THIS YET!" << std::endl;
    }

    for (auto& p : plugins) {
        dlclose(p);
    }
}
