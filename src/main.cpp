#include "simulator.h"
#include <iostream>

int main() {
    std::string programPath = "C:/Users/ASUS/Desktop/assembly_project/test/print_int.bin";  // Hardcoded file path

    Simulator sim;
    sim.run(programPath);

    return 0;
}
