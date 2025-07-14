#include "DataLoader.h"
#include <iostream>
#include <fstream>
#include <iomanip>

bool DataLoader::loadBinaryWithData(const std::string& filename, 
                                   std::vector<uint16_t>& instructions,
                                   DataSection& dataSection,
                                   Memory& mem) {
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        std::cerr << "Error: Cannot open file " << filename << std::endl;
        return false;
    }

    // For now, assume the file contains only instructions
    // In a real implementation, you'd parse the file format to separate code/data
    instructions.clear();
    
    while (true) {
        uint16_t word = 0;
        file.read(reinterpret_cast<char*>(&word), sizeof(word));
        if (file.gcount() < sizeof(word)) break;
        instructions.push_back(word);
    }

    // Initialize empty data section
    dataSection.start_address = 0x8000; // Default data start
    dataSection.data.clear();
    dataSection.labels.clear();

    return true;
}

bool DataLoader::loadDataSection(const std::string& filename, 
                                DataSection& dataSection,
                                Memory& mem) {
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        return false;
    }

    // Read data section into memory
    dataSection.data.clear();
    char byte;
    while (file.read(&byte, 1)) {
        dataSection.data.push_back(static_cast<uint8_t>(byte));
    }

    // Write data to memory
    for (size_t i = 0; i < dataSection.data.size(); i++) {
        mem.store8(dataSection.start_address + i, dataSection.data[i]);
    }

    return true;
}

void DataLoader::addStringToMemory(Memory& mem, uint16_t addr, const std::string& str) {
    try {
        for (size_t i = 0; i < str.length(); i++) {
            mem.store8(addr + i, static_cast<uint8_t>(str[i]));
        }
        // Add null terminator
        mem.store8(addr + str.length(), 0);
        
        std::cout << "Added string \"" << str << "\" at address 0x" 
                  << std::hex << addr << std::endl;
    } catch (const AddressOutOfBoundsException& e) {
        std::cerr << "Error adding string to memory: " << e.what() << std::endl;
    }
}

void DataLoader::addIntegerToMemory(Memory& mem, uint16_t addr, int16_t value) {
    try {
        mem.store16(addr, static_cast<uint16_t>(value));
        std::cout << "Added integer " << value << " at address 0x" 
                  << std::hex << addr << std::endl;
    } catch (const AddressOutOfBoundsException& e) {
        std::cerr << "Error adding integer to memory: " << e.what() << std::endl;
    }
}

void DataLoader::printMemoryRegion(const Memory& mem, uint16_t start, uint16_t length) {
    std::cout << "\n=== MEMORY REGION DUMP ===" << std::endl;
    std::cout << "Start: 0x" << std::hex << start << ", Length: " << std::dec << length << std::endl;
    
    for (uint16_t i = 0; i < length; i += 16) {
        std::cout << std::hex << std::setw(4) << std::setfill('0') << (start + i) << ": ";
        
        // Print hex bytes
        for (int j = 0; j < 16 && (i + j) < length; j++) {
            try {
                uint8_t byte = mem.load8(start + i + j);
                std::cout << std::hex << std::setw(2) << std::setfill('0') << (int)byte << " ";
            } catch (const AddressOutOfBoundsException&) {
                std::cout << "?? ";
            }
        }
        
        // Print ASCII representation
        std::cout << " |";
        for (int j = 0; j < 16 && (i + j) < length; j++) {
            try {
                uint8_t byte = mem.load8(start + i + j);
                char c = (byte >= 32 && byte <= 126) ? byte : '.';
                std::cout << c;
            } catch (const AddressOutOfBoundsException&) {
                std::cout << '?';
            }
        }
        std::cout << "|" << std::endl;
    }
    std::cout << "==========================" << std::endl;
}

void DataLoader::defineSolidTile(Memory& mem, uint8_t tileIndex, uint8_t color4bit) {
    if (tileIndex >= 16) {
        std::cerr << "Invalid tile index: " << (int)tileIndex << std::endl;
        return;
    }

    uint16_t tileAddr = 0xF200 + tileIndex * 128;
    uint8_t packed = (color4bit & 0x0F) | ((color4bit & 0x0F) << 4);  // two pixels per byte

    for (int i = 0; i < 128; ++i) {
        mem.store8(tileAddr + i, packed);
    }

    std::cout << "Defined tile " << (int)tileIndex << " filled with color " << std::hex << (int)color4bit << std::endl;
}

void DataLoader::fillTileMap(Memory& mem, uint8_t tileIndex) {
    if (tileIndex >= 16) {
        std::cerr << "Invalid tile index: " << (int)tileIndex << std::endl;
        return;
    }

    // Screen: 20 x 15 tiles (row-major)
    for (int y = 0; y < 15; ++y) {
        for (int x = 0; x < 20; ++x) {
            uint16_t addr = 0xF000 + y * 20 + x;

            // Fill only top-left 5×3 area for test
            if (x < 5 && y < 3) {
                mem.store8(addr, tileIndex);
            } else {
                mem.store8(addr, 0xFF);  // unused tile (transparent / invalid)
            }
        }
    }

    std::cout << "Filled top-left 5x3 tile area with tile " << (int)tileIndex << std::endl;
}
