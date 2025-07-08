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