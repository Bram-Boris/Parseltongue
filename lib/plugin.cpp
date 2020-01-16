#include "parseltongue/plugin.hpp"
#include "parseltongue/exceptions/plugin_exception.hpp"
#include <dlfcn.h>

Plugin::Plugin(std::string plugin_path) {
    handle = dlopen(plugin_path.c_str(), RTLD_LAZY);

    char* error = dlerror();
    if (error)
        throw PluginException("Could not load plugin module " + plugin_path + "\n" + error);

    create_file_format_fn_ptr = reinterpret_cast<create_file_format_fn>(dlsym(handle, "create_file_format"));
    get_file_extensions_fn_ptr = reinterpret_cast<get_file_extensions_fn>(dlsym(handle, "get_file_extensions"));
    get_file_format_name_fn_ptr = reinterpret_cast<get_file_format_name_fn>(dlsym(handle, "get_file_format_name"));

    error = dlerror();
    if (error)
        throw PluginException("Could not load plugin functions " + plugin_path + "\n" + error);
}

Plugin::~Plugin() {
    dlclose(handle);
}