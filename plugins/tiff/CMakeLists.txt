add_library(Parseltongue_Tiff MODULE
    lib.cpp
    tiff.cpp)

target_link_options(Parseltongue_Tiff
    PRIVATE -Wall -Wextra -pedantic -Werror)

target_link_libraries(Parseltongue_Tiff PUBLIC ParseltongueLib)
target_include_directories(Parseltongue_Tiff PUBLIC ./)

target_compile_features(Parseltongue_Tiff PUBLIC cxx_std_17)

