#include <iostream>
#include <unicode/ucnv.h>
#include <fstream>
#include <bitset>
#include <getopt.h>
#include <dlfcn.h>
#include <filesystem>
#include <functional>

#include "parseltongue/file_format.hpp"

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
    {"help", no_argument, nullptr, 'h'}
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
        const auto opt = getopt_long(argc, argv, "f:rw:h", long_options, nullptr);
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
            case 'h':
                std::cout << "There is no help! Go figure it out." << std::endl;
                exit(1);
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
    for(auto& p : std::filesystem::recursive_directory_iterator("./plugins")) {
        if (p.path().extension() == plugin_ext) {
            plugin_paths.push_back(p.path().string());
            std::cout << "Found plugin " << plugin_paths.back() << std::endl;
        }
    }

    std::vector<void*> plugins;
    for (auto& p : plugin_paths) {
        void* handle = dlopen(p.c_str(), RTLD_LAZY);
        const char* error = dlerror();
        if (!error) {
            plugins.push_back(handle);
            std::cout << "Succesfully loaded plugin " << p << std::endl;
        }
        else
            std::cout << "A dynamic linking error occurred: " << error << std::endl;
    }

    {
        auto [file_path, mode_with_text] = process_input(argc, argv);
        auto [mode, write_text] = mode_with_text;
        // TODO: match extension to possible file types
        std::filesystem::path file = std::filesystem::path(file_path);
        std::string file_ext = file.extension();
        std::cout << file_ext << std::endl;

        std::optional<std::unique_ptr<FileFormat>> ff_opt = std::nullopt;

        for (auto& p : plugins) {
            auto get_ext_fn_ptr = (std::vector<std::string>(*)())dlsym(p, "get_file_extensions");
            auto create_file_format_fn_ptr = (std::unique_ptr<FileFormat>(*)(std::string))dlsym(p, "create_file_format");
            for (auto& ext : get_ext_fn_ptr()) {
                if (ext == file_ext) {
                    ff_opt = create_file_format_fn_ptr(file.string());
                }
            }
        }

        if(!ff_opt) {
            std::cerr << "This extension is not implemented!" << std::endl;
            std::cerr << "You might need a plugin for this. Go ask Crabbe and Goyle." << std::endl;
            exit(1);
        }
        std::unique_ptr<FileFormat> ff = std::move(*ff_opt);

        if (mode == mode::READ) {
            ff->print_header();
            auto messages = ff->read_parseltongue();
            std::cout << "Amount of messages found: " << messages.size() << std::endl;
            for (auto& m : messages) {
                std::cout << "A message has been found: " << std::endl;
                std::cout << m << std::endl;
            }
        } else if (mode == mode::WRITE) {
            std::cerr << "WWEE DOOO NOTTTT SUUOOPOPPORT THIS YET!" << std::endl;
        }
    }

    for (auto& p : plugins) {
        dlclose(p);
    }
}
