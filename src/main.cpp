#include <iostream>
#include <fstream>
#include <vector>
#include <iomanip>
#include <limits>
#include "Decoder.h"

using namespace std;

std::vector<uint16_t> readBinaryFile(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    std::vector<uint16_t> instructions;

    if (!file) {
        std::cerr << "Error: Cannot open file " << filename << std::endl;
        return instructions;
    }

    while (true) {
        uint16_t word = 0;
        file.read(reinterpret_cast<char*>(&word), sizeof(word));
        if (file.gcount() < sizeof(word)) break;
        instructions.push_back(word);
    }

    return instructions;
}

void printDecodedInstruction(const DecodedInstruction& d, uint16_t pc) {
    std::cout << std::hex << std::setw(4) << std::setfill('0') << pc << ": " << d.mnemonic;

    switch (d.format) {
        case FORMAT_R:
            std::cout << " x" << (int)d.rd << ", x" << (int)d.rs2;
            break;
        case FORMAT_I:
            std::cout << " x" << (int)d.rd << ", " << std::dec << static_cast<int16_t>(d.imm);
            break;
        case FORMAT_B: {
            int16_t offset = static_cast<int16_t>(d.imm);
            uint16_t target = pc + 2 + offset;
            std::cout << " x" << (int)d.rs1 << ", x" << (int)d.rs2
                      << ", offset " << std::dec << offset
                      << " (0x" << std::hex << std::setw(4) << std::setfill('0') << target << ")";
            break;
        }
        case FORMAT_S: {
            int16_t offset = static_cast<int16_t>(d.imm);
            std::cout << " x" << (int)d.rs2 << ", " << std::dec << offset
                      << "(x" << (int)d.rs1 << ")";
            break;
        }
        case FORMAT_L: {
            int16_t offset = static_cast<int16_t>(d.imm);
            std::cout << " x" << (int)d.rd << ", " << std::dec << offset
                      << "(x" << (int)d.rs2 << ")";
            break;
        }
        case FORMAT_J: {
            int16_t offset = static_cast<int16_t>(d.imm);
            uint16_t target = pc + 2 + offset;
            std::cout << " x" << (int)d.rd << ", offset " << std::dec << offset
                      << " (0x" << std::hex << std::setw(4) << std::setfill('0') << target << ")";
            break;
        }
        case FORMAT_U:
            std::cout << " x" << std::dec << (int)d.rd
                      << ", 0x" << std::hex << d.imm
                      << " (" << std::dec << d.raw_imm << ")";
            break;
        case FORMAT_SYS:
            std::cout << " svc " << std::hex << d.imm;
            break;
        default:
            std::cout << " (unknown format)";
            break;
    }

    std::cout << std::endl;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: ZX16sim <program.bin>" << std::endl;
        return 1;
    }

    std::string programPath = argv[1];
    std::vector<uint16_t> instructions = readBinaryFile(programPath);  // ✅ Restore this

    if (instructions.empty()) {
        std::cerr << "No instructions loaded, exiting." << std::endl;
        return 1;
    }

    Decoder decoder;

    // Skip leading NOPs
    size_t startIndex = 0;
    while (startIndex < instructions.size() && instructions[startIndex] == 0x0000)
        ++startIndex;

    for (size_t i = startIndex; i < instructions.size(); ++i) {
        uint16_t pc = static_cast<uint16_t>(i * 2);
        DecodedInstruction d = decoder.decode(instructions[i]);

        printDecodedInstruction(d, pc);

        if (d.format == FORMAT_SYS && d.mnemonic == "ECALL") {
            std::cout << "ECALL encountered, stopping disassembly." << std::endl;
            break;
        }

        std::cout << "Press Enter to continue...";
        std::string dummy;
        std::getline(std::cin, dummy);
    }

    return 0;
}
