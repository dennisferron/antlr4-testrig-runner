cmake_minimum_required(VERSION 3.27)
project(antlr4-testrig-runner)

add_executable(antlr4-testrig-runner test_rig_main.cpp)

function(DLF_example_function foo bar)
    message("DLF_example_function: ${foo}, ${bar}")
endfunction()
