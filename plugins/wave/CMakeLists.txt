add_library(Parseltongue_Wave MODULE
    lib.cpp
    wave.cpp)

target_link_options(Parseltongue_Wave
    PRIVATE -Wall -Wextra -pedantic -Werror)

target_link_libraries(Parseltongue_Wave PUBLIC ParseltongueLib)
target_include_directories(Parseltongue_Wave PUBLIC ./)

target_compile_features(Parseltongue_Wave PUBLIC cxx_std_17)
