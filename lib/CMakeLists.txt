file(GLOB_RECURSE PARSELTONGUE_SRC_FILES RELATIVE ${CMAKE_CURRENT_LIST_DIR} *.c[p]*)

add_library(ParseltongueLib STATIC
    ${PARSELTONGUE_SRC_FILES}
)
## Compiler options
# Make the compiler complain, a lot
target_link_options(ParseltongueLib
    PRIVATE -Wall -Wextra -pedantic -Werror)

target_link_options(Parseltongue PRIVATE -ldl)
target_include_directories(ParseltongueLib PUBLIC ../include)

target_compile_features(ParseltongueLib PUBLIC cxx_std_17)