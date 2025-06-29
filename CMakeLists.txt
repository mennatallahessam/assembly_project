cmake_minimum_required(VERSION 3.29)
project(assembly_project)

set(CMAKE_CXX_STANDARD 14)

include_directories(src)

add_executable(assembly_project
    src/decoder.cpp
    src/decoder.h
    src/ecalls.cpp
    src/ecalls.h
    src/main.cpp
    "initial code.cpp"
    zx_16_simulator.cpp
        src/alu.h
        src/alu.cpp
        src/memory.h
        src/memory.cpp
        src/registers.h
        src/registers.cpp
        src/simulator.h
        src/simulator.cpp
        src/utils.h
        src/utils.cpp)
