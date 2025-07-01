#include "registers.h"
#include <iostream>
#include <iomanip>
#include <algorithm>

// RISC-V style register names for ZX16
const std::array<std::string, 32> Registers::register_names = {
    "zero", "ra", "sp", "gp", "tp", "t0", "t1", "t2",
    "s0", "s1", "a0", "a1", "a2", "a3", "a4", "a5",
    "a6", "a7", "s2", "s3", "s4", "s5", "s6", "s7",
    "s8", "s9", "s10", "s11", "t3", "t4", "t5", "t6"
};

Registers::Registers() : pc(0) {
    reset();
}

void Registers::validateIndex(int idx) const {
    if (idx < 0 || idx >= NUM_REGISTERS) {
        throw RegisterException("Register index " + std::to_string(idx) + " out of range [0-" + std::to_string(NUM_REGISTERS-1) + "]");
    }
}

uint32_t& Registers::operator[](int idx) {
    validateIndex(idx);
    // x0 (zero register) should always return 0 and ignore writes
    if (idx == 0) {
        static uint32_t zero_reg = 0;
        zero_reg = 0; // Ensure it's always 0
        return zero_reg;
    }
    return regs[idx];
}

uint32_t Registers::operator[](int idx) const {
    validateIndex(idx);
    // x0 (zero register) always returns 0
    if (idx == 0) {
        return 0;
    }
    return regs[idx];
}

uint32_t Registers::getRegister(const std::string& name) const {
    int idx = getRegisterIndex(name);
    return (*this)[idx];
}

void Registers::setRegister(const std::string& name, uint32_t value) {
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
    // Initialize all registers to zero
    regs.fill(0);
    pc = 0;
    // Note: x0 (zero register) will always read as 0 due to operator[] implementation
}

void Registers::dump() const {
    std::cout << "=== ZX16 Register Dump ===" << std::endl;
    std::cout << "PC: 0x" << std::hex << std::setw(8) << std::setfill('0') << pc << std::dec << std::endl;
    std::cout << std::endl;
    
    // Print registers in a nice 4-column format
    for (int i = 0; i < NUM_REGISTERS; i += 4) {
        for (int j = 0; j < 4 && (i + j) < NUM_REGISTERS; ++j) {
            int reg_idx = i + j;
            std::cout << "x" << std::setw(2) << std::setfill(' ') << reg_idx 
                      << "(" << std::setw(4) << register_names[reg_idx] << "): 0x"
                      << std::hex << std::setw(8) << std::setfill('0') << (*this)[reg_idx] << std::dec;
            if (j < 3 && (i + j + 1) < NUM_REGISTERS) {
                std::cout << "  ";
            }
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}
