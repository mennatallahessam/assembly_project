// File: src/utils.h
#ifndef UTILS_H
#define UTILS_H

#include <cstdint>

uint16_t toLittleEndian(uint8_t low, uint8_t high);

inline int16_t signExtend7(uint8_t imm7);

#endif
