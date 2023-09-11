cmake_minimum_required(VERSION 3.0)
project(compiler)

# Set the build type to Debug
set(CMAKE_BUILD_TYPE Debug)

# Add your source files to the executable
add_executable(compiler main.cpp jack_tokenizer.cpp symbol_table.cpp vm_writer.cpp compilation_engine.cpp)
