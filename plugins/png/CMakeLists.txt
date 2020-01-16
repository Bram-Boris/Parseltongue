add_library(Parseltongue_PNG MODULE
    lib.cpp
    png.cpp)

target_link_options(Parseltongue_PNG
    PRIVATE -Wall -Wextra -pedantic -Werror)

find_package(ZLIB REQUIRED)

target_link_libraries(Parseltongue_PNG 
    PUBLIC ParseltongueLib
    PUBLIC ZLIB::ZLIB
)
target_include_directories(Parseltongue_PNG PUBLIC ./)

target_compile_features(Parseltongue_PNG PUBLIC cxx_std_17)
