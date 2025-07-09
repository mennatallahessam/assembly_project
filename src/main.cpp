#include <iostream>
#include <fstream>
#include <vector>
#include <iomanip>
#include <limits>

#include "Decoder.h"
#include "Registers.h"
#include "Memory.h"
#include "graphics.h"
#include "Ecalls.h"
#include "alu.h"
#include "DataLoader.h"  // Add this include

using namespace std;

// Keep your existing readBinaryFile function for compatibility
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


// Keep your existing formatInstruction function
std::string formatInstruction(const DecodedInstruction& d) {
    std::string result = d.mnemonic;

    // Check for NOP (ADD x0, x0, x0)
    if (d.format == FORMAT_R && d.mnemonic == "ADD" && d.rd == 0 && d.rs1 == 0 && d.rs2 == 0) {
        return "NOP";
    }

    switch (d.format) {
        case FORMAT_R:
            if (d.mnemonic == "MV") {
                result += " x" + std::to_string(d.rd) + ", x" + std::to_string(d.rs2);
            }
            else if (d.mnemonic == "JR") {
                result += " x" + std::to_string(d.rd);
            }
            else if (d.mnemonic == "JALR") {
                result += " x" + std::to_string(d.rd) + ", x" + std::to_string(d.rs2);
            }
            else {
                result += " x" + std::to_string(d.rd) + ", x" + std::to_string(d.rs2);
            }
            break;

        case FORMAT_I:
            result += " x" + std::to_string(d.rd) + ", " + std::to_string(d.imm);
            break;

        case FORMAT_S:
            result += " x" + std::to_string(d.rs2) + ", " + std::to_string(d.imm) + "(x" + std::to_string(d.rs1) + ")";
            break;

        case FORMAT_L:
            result += " x" + std::to_string(d.rd) + ", " + std::to_string(d.imm) + "(x" + std::to_string(d.rs2) + ")";
            break;

        case FORMAT_B:
            result += " x" + std::to_string(d.rs1) + ", x" + std::to_string(d.rs2) + ", " + std::to_string(d.imm);
            break;

        case FORMAT_J:
            if (d.rd != 0) {
                result += " x" + std::to_string(d.rd) + ", " + std::to_string(d.imm);
            } else {
                result += " " + std::to_string(d.imm);
            }
            break;

        case FORMAT_U:
            result += " x" + std::to_string(d.rd) + ", " + std::to_string(d.imm);
            break;

        case FORMAT_SYS:
            if (d.sys_op == SYSOP_ECALL) {
                result += " " + std::to_string(d.imm);
            }
            break;
    }

    return result;
}

// Helper function to wait for Enter key press
void waitForEnter() {
    std::cout << "Press Enter to continue..." << std::endl;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}
// Add this to your simulator to manually create test strings for debugging
/*void setupTestStrings(Memory& mem) {
    // Place strings at known addresses for testing

    DataLoader::addStringToMemory(mem, 0x8000, "Hello World!");
    DataLoader::addStringToMemory(mem, 0x8010, "ZX16 Simulator");
    DataLoader::addStringToMemory(mem, 0x8020, "Testing string printing");
    DataLoader::addStringToMemory(mem, 0x8040, "CSCE 2303");
    DataLoader::addIntegerToMemory(mem, 0x8050, 42);
    DataLoader::addIntegerToMemory(mem, 0x8052, -123);

}*/

// Call this after loading your binary file in main()
//
int main() {
    Decoder decoder;
    Registers regs;
    Memory mem;
    graphics gfx;
    Ecalls ecalls;
    ALU alu;
    DataSection dataSection;

    //setupTestStrings(mem);
    std::string programPath = "C:/Users/ASUS/Desktop/z16-fork/assembler/lasttest.bin";
    std::vector<uint16_t> instructions = readBinaryFile(programPath);

    if (instructions.empty()) {
        std::cerr << "No instructions loaded, exiting." << std::endl;
        return 1;
    }


    // Initialize test data in memory
   // initializeTestData(mem);

    // Load instructions into memory starting at 0x0000
    for (size_t i = 0; i < instructions.size(); ++i) {
        mem.store16(i * 2, instructions[i]);
    }
   // setupTestStrings(mem);
    // Optional: Print memory regions for debugging
 /*   std::cout << "\n=== MEMORY INITIALIZATION DEBUG ===" << std::endl;
    std::cout << "Instructions loaded at 0x0000-0x" << std::hex << (instructions.size() * 2) << std::endl;
    std::cout << "Data section starts at 0x8000" << std::endl;*/
    DataLoader::printMemoryRegion(mem, 0x8000, 0x200);  // Print first 512 bytes of data

    uint16_t pc = 0;
    bool halted = false;

    std::cout << "\nStarting program execution. Press Enter after each instruction to continue.\n" << std::endl;

    while (!halted) {
        uint16_t inst = mem.load16(pc);
        DecodedInstruction d = decoder.decode(inst);

        // Check if this is a NOP and skip display if desired
        bool is_nop = (d.format == FORMAT_R && d.mnemonic == "ADD" && d.rd == 0 && d.rs1 == 0 && d.rs2 == 0);

        // Display instruction (skip NOPs)
        if (!is_nop) {
            std::string formatted = formatInstruction(d);
            std::cout << std::hex << std::setw(4) << std::setfill('0') << pc << ": " << formatted << std::endl;
        }

        // Handle ECALL specially since it needs syscall_num set
        if (d.format == FORMAT_SYS && d.sys_op == SYSOP_ECALL) {
            DecodedInstruction ecall_instr = d;
            ecall_instr.syscall_num = d.imm;

            // Debug output for ECALL
           /* if (d.imm == 3) {  // Print string
                std::cout << "[DEBUG] Print string ECALL: a0=0x" << std::hex << regs[6]
                          << " (" << std::dec << regs[6] << ")" << std::endl;
            }*/

            ecalls.handle(ecall_instr, regs, mem, halted, gfx);
            pc += 2;
        } else {
            // Store current PC for ALU execution
            uint16_t next_pc = pc + 2;

            // Execute instruction using ALU
            alu.execute(d, regs, mem, next_pc, halted, ecalls, gfx);

            // Update PC
            pc = next_pc;
        }
        regs.setPC(pc);

        // Wait for Enter key press after each instruction (except NOPs)
        if (!is_nop && !halted) {
            waitForEnter();
        }
    }

    std::cout << "\nProgram execution completed." << std::endl;
    return 0;
}