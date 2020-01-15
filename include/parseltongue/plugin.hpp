#ifndef FILE_PLUGIN_HPP
#define FILE_PLUGIN_HPP

#include <string>
#include <memory>
#include <vector>

#include "parseltongue/file_format.hpp"

class Plugin {
public:
    Plugin(std::string plugin_path);
    ~Plugin();

    std::unique_ptr<FileFormat> create_file_format(std::string file_path) {
        return create_file_format_fn_ptr(file_path);
    }
    std::vector<std::string> get_file_extensions() {
        return get_file_extensions_fn_ptr();
    }
    std::string get_file_format_name() {
        return get_file_format_name_fn_ptr();
    }
private:
    void* handle;
    using create_file_format_fn = std::unique_ptr<FileFormat>(*)(std::string);
    create_file_format_fn create_file_format_fn_ptr;
    using get_file_extensions_fn = std::vector<std::string>(*)();
    get_file_extensions_fn get_file_extensions_fn_ptr;
    using get_file_format_name_fn = std::string (*)();
    get_file_format_name_fn get_file_format_name_fn_ptr;
};

#endif // FILE_PLUGIN_HPP