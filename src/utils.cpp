// File: src/utils.cpp
#include "utils.h"

uint16_t toLittleEndian(uint8_t low, uint8_t high) {
    return (high << 8) | low;
}
