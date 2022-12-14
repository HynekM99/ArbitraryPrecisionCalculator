cmake_minimum_required(VERSION 3.5)

project(main LANGUAGES C)
set(CMAKE_C_FLAGS "-Wall -Wextra -pedantic -ansi")

add_executable(main 
    src/main.c 
    
    src/vector.c
    src/stack/stack.c
    src/operators.c
    src/shunting_yard.c
    src/mpt/multiple_precision_type.c
    src/mpt/multiple_precision_parsing.c
    src/mpt/multiple_precision_printing.c
    src/mpt/multiple_precision_operations.c
)

add_executable(tests
    src/tests.c 
    
    src/vector.c
    src/stack/stack.c
    src/operators.c
    src/shunting_yard.c
    src/mpt/multiple_precision_type.c
    src/mpt/multiple_precision_parsing.c
    src/mpt/multiple_precision_printing.c
    src/mpt/multiple_precision_operations.c
)
