#ifndef DATA_LOADER_H
#define DATA_LOADER_H

#include <string>
#include <vector>
#include <map>
#include "memory.h"

struct DataSection {
    uint16_t start_address;
    std::vector<uint8_t> data;
    std::map<std::string, uint16_t> labels; // For debugging/symbol table
};

class DataLoader {
public:
    // Load data from a binary file that contains both code and data sections
    static bool loadBinaryWithData(const std::string& filename,
                                   std::vector<uint16_t>& instructions,
                                   DataSection& dataSection,
                                   Memory& mem);

    // Load data section from a separate data file
    static bool loadDataSection(const std::string& filename,
                               DataSection& dataSection,
                               Memory& mem);

    // Manually add string data to memory (for testing)
    static void addStringToMemory(Memory& mem, uint16_t addr, const std::string& str);

    // Add integer data to memory
    static void addIntegerToMemory(Memory& mem, uint16_t addr, int16_t value);

    // Print memory contents for debugging
    static void printMemoryRegion(const Memory& mem, uint16_t start, uint16_t length);

    void defineSolidTile(Memory &mem, uint8_t tileIndex, uint8_t color4bit);

    void fillTileMap(Memory& mem, uint8_t tileIndex);
};
#endif