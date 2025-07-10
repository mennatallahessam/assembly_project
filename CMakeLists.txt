cmake_minimum_required(VERSION 3.29)
project(assembly_project)

set(CMAKE_CXX_STANDARD 14)

# Set SFML_DIR to the MinGW build's cmake folder
set(SFML_DIR "C:/Users/ASUS/Desktop/assembly_project/SFML-2.6.1-windows-gcc-13.1.0-mingw-64-bit/SFML-2.6.1/lib/cmake/SFML")

find_package(SFML 2.6 COMPONENTS graphics window system REQUIRED)

include_directories(src)

add_executable(assembly_project
        src/decoder.cpp
        src/ecalls.cpp
        src/main.cpp
        "initial code.cpp"
        zx_16_simulator.cpp
        src/alu.cpp
        src/memory.cpp
        src/registers.cpp
        src/utils.cpp
        src/DataLoader.cpp
        src/graphics.cpp
)

target_link_libraries(assembly_project sfml-graphics sfml-window sfml-system)
