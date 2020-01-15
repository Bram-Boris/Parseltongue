set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

add_executable(Parseltongue
    main.cpp
)

## Compiler options
# Turn on the address sanitizer for safety checks
option(PARSELTONGUE_SANITIZER "Turn on the address sanitizer" OFF)
if(PARSELTONGUE_SANITIZER)
    target_compile_options(Parseltongue
        PRIVATE -fsanitize=address -fno-omit-frame-pointer)
    target_link_options(Parseltongue
        PRIVATE -fsanitize=address)
endif()
# Make the compiler complain, a lot
target_compile_options(Parseltongue
    PRIVATE -Wall -Wextra -pedantic -Werror)
target_compile_features(Parseltongue PUBLIC cxx_std_17)

target_link_libraries(Parseltongue PRIVATE ParseltongueLib)

add_custom_target(Parseltongue_Data ALL
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                       ${CMAKE_SOURCE_DIR}/data/ $<TARGET_FILE_DIR:Parseltongue>/data)

