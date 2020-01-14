file(GLOB_RECURSE files RELATIVE ${CMAKE_CURRENT_LIST_DIR} *.c[p]*)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

## Compiler options
# Make the compiler complain, a lot
# Force colors when using ninja with clang
if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    add_compile_options(-fcolor-diagnostics)
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    add_compile_options(-fdiagnostics-color=always)
endif()
# Turn on the address sanitizer for safety checks
option(SANITIZER "Turn on the address sanitizer" OFF)
if(SANITIZER)
    add_compile_options(-fsanitize=address -fno-omit-frame-pointer -O1)
    add_link_options(-fsanitize=address)
endif()
add_compile_options(-Wall -Wextra -pedantic)

add_executable(Parseltongue
    ${files}
)
target_include_directories(Parseltongue PUBLIC ../include)
target_compile_features(Parseltongue PUBLIC cxx_std_17)

target_link_options(Parseltongue PRIVATE -ldl)

add_custom_command(TARGET Parseltongue PRE_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                       ${CMAKE_SOURCE_DIR}/data/ $<TARGET_FILE_DIR:Parseltongue>/data)
