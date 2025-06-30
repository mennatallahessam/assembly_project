#include "simulator.h"
#include <iostream>

int main() {
    std::string programPath = "C:/Users/ASUS/Desktop/assembly_project/test/ecall_test2.bin";  // Hardcoded file path
    Simulator sim;
    sim.run(programPath);

    return 0;
}
