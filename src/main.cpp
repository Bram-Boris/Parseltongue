#include <iostream>
#include <unicode/ucnv.h>
#include <fstream>
#include <bitset>
#include <getopt.h>
#include <dlfcn.h>
#include <filesystem>
#include <functional>

#include "parseltongue/file_format.hpp"
#include "parseltongue/mode.hpp"
#include "parseltongue/plugin.hpp"

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
                    if (wm->size() > 2000) {
                        std::cerr << "Your message may not be longer than 2000 characters" << std::endl;
                        exit(1);
                    }
                }
                else {
                    std::cerr << "You need to supply a secret when writing" << std::endl;
                    exit(1);
                }
                break;
            case 'h':
                std::cout << "There is no help! Go figure it out." << std::endl;
                exit(1);
               break;
        }
    }
    if (!m || !f) {
        std::cerr << "You need to supply a file and a mode" << std::endl;
        exit(1);
    }
    if(m == mode::WRITE) {
        size_t size = std::filesystem::file_size(*f);
        if (size < (wm->size() * 10)) {
            std::cerr << "Your message is too long for the file." << std::endl;
            exit(1);
        }
    }

    return std::make_pair(*f, std::make_pair(*m, wm));
}

const char* plugin_ext = ".so";

int main(int argc, char** argv) {
    int exit_code = 0;
    try {
        std::cout << "Your processor has a " << (su::cur_byte_order() == su::byte_order::little_endian ? "little" : "big") << " endian architecture\n";

        std::vector<std::string> plugin_paths;
        for(auto& p : std::filesystem::recursive_directory_iterator("./plugins")) {
            if (p.path().extension() == plugin_ext) {
                plugin_paths.push_back(p.path().string());
                std::cout << "Found plugin " << plugin_paths.back() << std::endl;
            }
        }

        std::vector<std::unique_ptr<Plugin>> plugins;
        for (auto& p_path : plugin_paths) {
            auto plugin = std::make_unique<Plugin>(p_path);
            plugins.push_back(std::move(plugin));
        }

        auto [file_path, mode_with_text] = process_input(argc, argv);
        auto [mode, write_message] = mode_with_text;
        // TODO: match extension to possible file types
        std::filesystem::path file = std::filesystem::path(file_path);
        std::string file_ext = file.extension();

        std::optional<std::unique_ptr<FileFormat>> ff_opt = std::nullopt;

        // TODO: exception handling
        for (auto& p : plugins) {
            for (auto& ext : p->get_file_extensions()) {
                if (ext == file_ext) {
                    ff_opt = p->create_file_format(file.string());
                }
            }
        }

        if(!ff_opt) {
            std::cerr << "This extension is not implemented!" << std::endl;
            std::cerr << "You might need a plugin for this. Go ask Crabbe and Goyle." << std::endl;
            exit(1);
        }
        std::unique_ptr<FileFormat> ff = std::move(*ff_opt);

        ff->print_header();
        if (mode == mode::READ) {
            ff->read_parseltongue();
        } else if (mode == mode::WRITE) {
            ff->speak_parseltongue(*write_message);
        }
    } catch(const std::runtime_error& ex) {
        std::cerr << "An exception was thrown: " << ex.what() << std::endl;
        exit_code = EXIT_FAILURE;
    } catch(const std::exception& ex) {
        std::cerr << "An exception was thrown: " << ex.what() << std::endl;
        exit_code = EXIT_FAILURE;
    } catch(...) {
        std::cerr << "An unknown error occured." << std::endl;
        exit_code = EXIT_FAILURE;
    }
    return exit_code;
}
