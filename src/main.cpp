#include "simulator.h"
#include <iostream>

int main() {
    std::string programPath = "C:/Users/AUC/Downloads/ecall_test2.bin";  // Hardcoded file path

    Simulator sim;
    sim.run(programPath);

    return 0;
}
