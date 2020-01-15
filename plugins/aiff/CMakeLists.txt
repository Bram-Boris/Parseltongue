add_library(Parseltongue_Aiff MODULE
    lib.cpp
    aiff.cpp)

target_link_options(Parseltongue_Aiff
    PRIVATE -Wall -Wextra -pedantic -Werror)

target_link_libraries(Parseltongue_Aiff PUBLIC ParseltongueLib)
target_include_directories(Parseltongue_Aiff PUBLIC ./)

target_compile_features(Parseltongue_Aiff PUBLIC cxx_std_17)
