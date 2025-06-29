#include "registers.h"
#include <iostream>
#include <iomanip>

Registers::Registers() {
    reset();
}

uint16_t& Registers::operator[](int idx) {
    return regs.at(idx); // use .at() for bounds checking (optional)
}

uint16_t Registers::operator[](int idx) const {
    return regs.at(idx);
}

void Registers::dump() const {
    std::cout << "=== Register Dump ===\n";
    for (int i = 0; i < 16; ++i) {
        std::cout << "R" << std::setw(2) << i << ": 0x"
                  << std::hex << std::setw(4) << std::setfill('0') << regs[i] << std::dec << "\n";
    }
}

void Registers::reset() {
    regs.fill(0);
}
