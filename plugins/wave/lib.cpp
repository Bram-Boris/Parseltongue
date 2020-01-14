#include "parseltongue/file_format.hpp"
#include "wave.hpp"
#include <memory>
#include <string>

extern "C" {
    std::unique_ptr<FileFormat> create_file_format(std::string file_path) {
        return std::make_unique<Wave>(file_path);
    }
    std::vector<std::string> get_file_extensions() {
        return std::vector<std::string> {
            ".wav",
            ".wave"
        };
    }
    std::string get_file_format_name() { return "WAVE"; }
}