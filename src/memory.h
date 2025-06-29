// Example for memory.h

#ifndef MEMORY_H
#define MEMORY_H

#include <cstdint>

class Memory {
public:
    Memory();
    uint8_t readByte(uint16_t addr) const;
    uint16_t readHalfWord(uint16_t addr) const;
    void writeByte(uint16_t addr, uint8_t val);
    void writeHalfWord(uint16_t addr, uint16_t val);

private:
    uint8_t data[65536];  // 64KB memory
};

#endif // MEMORY_H
