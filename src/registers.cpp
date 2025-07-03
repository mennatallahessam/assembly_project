#include "registers.h"
#include <iostream>
#include <iomanip>
#include <algorithm>

// ZX16 register names for 8 registers (matching ABI specification)
const std::array<std::string, 8> Registers::register_names = {
    "t0", "ra", "sp", "s0", "s1", "t1", "a0", "a1"
};

Registers::Registers() : pc(0) {
    reset();
}

void Registers::validateIndex(int idx) const {
    if (idx < 0 || idx >= NUM_REGISTERS) {
        throw RegisterException("Register index " + std::to_string(idx) + " out of range [0-" + std::to_string(NUM_REGISTERS-1) + "]");
    }
}

uint16_t& Registers::operator[](int idx) {
    validateIndex(idx);
    // x0 (t0) is a normal register in ZX16, not a zero register
    return regs[idx];
}

uint16_t Registers::operator[](int idx) const {
    validateIndex(idx);
    // x0 (t0) is a normal register in ZX16, not a zero register
    return regs[idx];
}

uint16_t Registers::getRegister(const std::string& name) const {
    int idx = getRegisterIndex(name);
    return (*this)[idx];
}

void Registers::setRegister(const std::string& name, uint16_t value) {
    int idx = getRegisterIndex(name);
    (*this)[idx] = value;
}

std::string Registers::getRegisterName(int idx) const {
    validateIndex(idx);
    return register_names[idx];
}

int Registers::getRegisterIndex(const std::string& name) const {
    // Check for numeric format (x0, x1, etc.)
    if (name.length() > 1 && name[0] == 'x') {
        try {
            int idx = std::stoi(name.substr(1));
            if (idx >= 0 && idx < NUM_REGISTERS) {
                return idx;
            }
        } catch (...) {
            // Fall through to name-based lookup
        }
    }

    // Check for ABI names
    auto it = std::find(register_names.begin(), register_names.end(), name);
    if (it != register_names.end()) {
        return static_cast<int>(it - register_names.begin());
    }

    throw RegisterException("Unknown register name: " + name);
}

void Registers::reset() {
    // Initialize all registers to undefined values (ZX16 spec: registers undefined on reset)
    // We'll initialize to 0 for predictable behavior in simulation
    regs.fill(0);
    pc = 0;  // PC initialized to 0x0000 on reset

    // Initialize stack pointer to 0xEFFE as per ZX16 specification
    regs[2] = 0xEFFE;  // x2 (sp) = 0xEFFE
}

void Registers::dump() const {
    std::cout << "=== ZX16 Register Dump ===" << std::endl;
    std::cout << "PC: 0x" << std::hex << std::setw(4) << std::setfill('0') << pc << std::dec << std::endl;
    std::cout << std::endl;

    // Print registers in a nice 4-column format
    for (int i = 0; i < NUM_REGISTERS; i += 4) {
        for (int j = 0; j < 4 && (i + j) < NUM_REGISTERS; ++j) {
            int reg_idx = i + j;
            std::cout << "x" << std::setw(1) << std::setfill(' ') << reg_idx
                      << "(" << std::setw(4) << register_names[reg_idx] << "): 0x"
                      << std::hex << std::setw(4) << std::setfill('0') << (*this)[reg_idx] << std::dec;
            if (j < 3 && (i + j + 1) < NUM_REGISTERS) {
                std::cout << "  ";
            }
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}