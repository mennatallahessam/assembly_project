// #include "memory.h"
// #include <stdexcept>
// #include <iostream>
//
// Memory::Memory() {
//     // Initialize all memory to zero
//     for (int i = 0; i < 65536; ++i) {
//         data[i] = 0;
//     }
// }
//
// uint8_t Memory::readByte(uint16_t addr) const {
//     return data[addr];
// }
//
// uint16_t Memory::readHalfWord(uint16_t addr) const {
//     if (addr >= 65535) {
//         throw std::out_of_range("Memory readHalfWord out of range");
//     }
//     // Little endian: low byte first
//     return data[addr] | (data[addr + 1] << 8);
// }
//
//
// void Memory::writeByte(uint16_t addr, uint8_t val) {
//     data[addr] = val;
// }
//
// void Memory::writeHalfWord(uint16_t addr, uint16_t val) {
//     if (addr >= 65535) {
//         throw std::out_of_range("Memory writeHalfWord out of range");
//     }
//     // Store as little endian
//     data[addr] = val & 0xFF;
//     data[addr + 1] = (val >> 8) & 0xFF;
// }
