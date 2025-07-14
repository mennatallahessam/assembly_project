#include "memory.h"
#include <cstring>
#include <stdexcept>

Memory::Memory() {
    reset();
}

void Memory::reset() {
    std::memset(data, 0, MEMORY_SIZE);  // 64KB zeroed out
}

void Memory::checkBounds(uint32_t addr, uint32_t size) const {
    if (addr >= MEMORY_SIZE || addr + size > MEMORY_SIZE) {
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
    if (addr % 2 != 0) {
        throw AddressMisalignedException(addr, 2);
    }
    checkBounds(addr, 2);
    return static_cast<uint16_t>(data[addr]) |
           (static_cast<uint16_t>(data[addr + 1]) << 8);
}

void Memory::writeHalfWord(uint32_t addr, uint16_t val) {
    if (addr % 2 != 0) {
        throw AddressMisalignedException(addr, 2);
    }
    checkBounds(addr, 2);
    data[addr]     = static_cast<uint8_t>(val & 0xFF);
    data[addr + 1] = static_cast<uint8_t>((val >> 8) & 0xFF);
}

// ZX16-compatible aliases
uint16_t Memory::load16(uint32_t addr) const {
    return readHalfWord(addr);
}

uint8_t Memory::load8(uint32_t addr) const {
    return readByte(addr);
}

// In memory.cpp - Add graphics update detection
// In memory.cpp, store8() method, add graphics debugging:
void Memory::store8(uint32_t addr, uint8_t val) {
    checkBounds(addr, 1);
    data[addr] = val;

    // Debug ALL graphics memory writes
    if (addr >= 0xF000 && addr <= 0xF12B) {
        std::cout << "[ASSEMBLY GRAPHICS] Writing tile " << (int)val
                  << " to (" << ((addr-0xF000)%20) << "," << ((addr-0xF000)/20)
                  << ") at 0x" << std::hex << addr << std::dec << std::endl;
    }
}
void Memory::store16(uint32_t addr, uint16_t val) {
    if (addr % 2 != 0) {
        throw AddressMisalignedException(addr, 2);
    }
    checkBounds(addr, 2);
    data[addr]     = static_cast<uint8_t>(val & 0xFF);
    data[addr + 1] = static_cast<uint8_t>((val >> 8) & 0xFF);

    // Check if write is to graphics memory region
    if (addr >= 0xF000 && addr <= 0xFFFF) {
       // std::cout << "[GRAPHICS] Memory write to graphics region: 0x"
         //         << std::hex << addr << " = 0x" << val << std::endl;
    }
}
