#ifndef MEMORY_H
#define MEMORY_H

#include <cstdint>
#include <stdexcept>
#include <iostream>
#include <fstream>
#include <vector>
#include <iomanip>
#include <string>
#include <stdexcept>
#include <cstring>
#include <map>
#include <SFML/Graphics.hpp>
const uint32_t MEMORY_SIZE = 65536; // 64KB address space

// Custom exception classes
class AddressOutOfBoundsException : public std::runtime_error {
public:
    AddressOutOfBoundsException(uint32_t addr)
        : std::runtime_error("Address out of bounds: 0x" + std::to_string(addr)) {}
};

class AddressMisalignedException : public std::runtime_error {
public:
    AddressMisalignedException(uint32_t addr, uint32_t alignment)
        : std::runtime_error("Address misaligned: 0x" + std::to_string(addr) +
                           " (required alignment: " + std::to_string(alignment) + ")") {}
};

class Memory {
private:
    uint8_t data[MEMORY_SIZE];

    void checkBounds(uint32_t addr, uint32_t size) const;

public:
    Memory();

    void reset();

    // Original method names (descriptive)
    uint8_t readByte(uint32_t addr) const;
    void writeByte(uint32_t addr, uint8_t val);

    uint16_t readHalfWord(uint32_t addr) const;
    void writeHalfWord(uint32_t addr, uint16_t val);

    uint16_t readWord(uint32_t addr) const;
    void writeWord(uint32_t addr, uint16_t val);

    // Compatibility methods for main() function
    uint16_t load16(uint32_t addr) const;
    void store16(uint32_t addr, uint16_t val);

    uint8_t load8(uint32_t addr) const;
    void store8(uint32_t addr, uint8_t val);

    // uint32_t load32(uint32_t addr) const;
    // void store32(uint32_t addr, uint32_t val);
};

#endif // MEMORY_H
