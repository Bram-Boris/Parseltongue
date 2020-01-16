#include "parseltongue/file_format.hpp"
#include "png.hpp"
#include <memory>
#include <string>

#pragma clang diagnostic ignored "-Wreturn-type-c-linkage"
extern "C" {
    std::unique_ptr<FileFormat> create_file_format(std::string file_path) {
        return std::make_unique<PNG>(file_path);
    }
    std::vector<std::string> get_file_extensions() {
        return std::vector<std::string> {
            ".png"
        };
    }
    std::string get_file_format_name() { return "PNG"; }
}