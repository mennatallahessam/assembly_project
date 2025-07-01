// File: src/utils.cpp
#include "utils.h"

uint16_t toLittleEndian(uint8_t low, uint8_t high) {
    return (high << 8) | low;
}

int16_t signExtend7(uint8_t imm7) {
    if (imm7 & 0x40)  // if bit 6 (sign bit) is set
        return (int16_t)(imm7 | 0xFF80); // sign extend negative
    else
        return imm7 & 0x7F; // positive
}
