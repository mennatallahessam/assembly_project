// // ZX16 Memory Implementation
// // Team: Mennatallah Zaher, Mennatallah Zaid, Ahmed Mohamed Elfishawy
//
// #include "memory.h"
// #include <fstream>
// #include <iostream>
// #include <iomanip>
// #include <stdexcept>
//
// Memory::Memory() : memory(MEMORY_SIZE, 0) {
//     // Initialize memory to zero
//     clear();
// }
//
// Memory::~Memory() {
//     // Cleanup if needed
// }
//
// void Memory::clear() {
//     std::fill(memory.begin(), memory.end(), 0);
// }
//
// uint8_t Memory::readByte(uint32_t address) const {
//     if (address >= MEMORY_SIZE) {
//         throw std::runtime_error("Memory read out of bounds: 0x" +
//                                 std::to_string(address));
//     }
//     return memory[address];
// }
//
// uint16_t Memory::readHalfWord(uint32_t address) const