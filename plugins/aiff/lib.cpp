#include "parseltongue/file_format.hpp"
#include "aiff.hpp"
#include <memory>
#include <string>

#pragma clang diagnostic ignored "-Wreturn-type-c-linkage"
extern "C" {
    std::unique_ptr<FileFormat> create_file_format(std::string file_path) {
        return std::make_unique<Aiff>(file_path);
    }
    std::vector<std::string> get_file_extensions() {
        return std::vector<std::string> {
            ".aif",
            ".aiff"
        };
    }
    std::string get_file_format_name() { return "AIFF"; }
}