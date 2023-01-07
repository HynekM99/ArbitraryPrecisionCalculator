cmake_minimum_required(VERSION 3.5)

project(calc LANGUAGES C)
set(CMAKE_C_FLAGS "-Wall -Wextra -pedantic -ansi")

add_executable(calc 
    src/calc.c 
    
    src/operators.c
    src/shunting_yard.c
    src/data_structures/stack.c
    src/data_structures/vector.c
    src/data_structures/conversion.c
    src/mpt/multiple_precision_type.c
    src/mpt/multiple_precision_parsing.c
    src/mpt/multiple_precision_printing.c
    src/mpt/multiple_precision_operations.c
)