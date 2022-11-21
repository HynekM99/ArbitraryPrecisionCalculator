cmake_minimum_required(VERSION 3.5)

project(main LANGUAGES C)
set(CMAKE_C_FLAGS "-Wall -Wextra -pedantic -ansi")

add_executable(main 
    src/main.c 
    
    src/array_list.c
    src/multiple_precision_type.c
)

add_executable(tests
    src/tests.c 
    
    src/array_list.c
    src/multiple_precision_type.c
)

target_link_libraries(main m)
