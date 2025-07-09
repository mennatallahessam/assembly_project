#include "memory.h"
#include <cstring>

Memory::Memory() {
    reset();
}

void Memory::reset() {
    // Initialize all memory to zero
    std::memset(data, 0, MEMORY_SIZE);
}

void Memory::checkBounds(uint32_t addr, uint32_t size) const {
    if (addr >= MEMORY_SIZE || (addr + size - 1) >= MEMORY_SIZE) {
        throw AddressOutOfBoundsException(addr);
    }
}

uint8_t Memory::readByte(uint32_t addr) const {
    checkBounds(addr, 1);
    return data[addr];
}

void Memory::writeByte(uint32_t addr, uint8_t val) {
    checkBounds(addr, 1);
    data[addr] = val;
}

uint16_t Memory::readHalfWord(uint32_t addr) const {
    // Check alignment for RV32I compliance
    if (addr % 2 != 0) {
        throw AddressMisalignedException(addr, 2);
    }
    checkBounds(addr, 2);
    
    // Little endian: low byte first
    return static_cast<uint16_t>(data[addr]) | 
           (static_cast<uint16_t>(data[addr + 1]) << 8);
}

void Memory::writeHalfWord(uint32_t addr, uint16_t val) {
    // Check alignment for RV32I compliance
    if (addr % 2 != 0) {
        throw AddressMisalignedException(addr, 2);
    }
    checkBounds(addr, 2);
    
    // Store as little endian
    data[addr] = static_cast<uint8_t>(val & 0xFF);
    data[addr + 1] = static_cast<uint8_t>((val >> 8) & 0xFF);
}

uint16_t Memory::readWord(uint32_t addr) const {
    if (addr % 2 != 0) {
        throw AddressMisalignedException(addr, 2);
    }
    checkBounds(addr, 2);

    return static_cast<uint16_t>(data[addr]) |
           (static_cast<uint16_t>(data[addr + 1]) << 8);
}

void Memory::writeWord(uint32_t addr, uint16_t val) {
    if (addr % 2 != 0) {
        throw AddressMisalignedException(addr, 2);
    }
    checkBounds(addr, 2);

    data[addr] = static_cast<uint8_t>(val & 0xFF);
    data[addr + 1] = static_cast<uint8_t>((val >> 8) & 0xFF);
}

// ===== COMPATIBILITY METHODS FOR MAIN FUNCTION =====
// These methods match the interface used in your main() function

uint16_t Memory::load16(uint32_t addr) const {
    return readHalfWord(addr);
}

void Memory::store16(uint32_t addr, uint16_t val) {
    writeHalfWord(addr, val);
}

uint8_t Memory::load8(uint32_t addr) const {
    return readByte(addr);
}

void Memory::store8(uint32_t addr, uint8_t val) {
    writeByte(addr, val);
}

// uint32_t Memory::load32(uint32_t addr) const {
//     return readWord(addr);
// }

// void Memory::store32(uint32_t addr, uint32_t val) {
//     writeWord(addr, val);
// }
