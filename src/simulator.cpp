#include "simulator.h"
#include <fstream>
#include <iostream>
#include <iomanip>

Simulator::Simulator()
    : ecalls(mem, regs), pc(0), halted(false) {}

void Simulator::run(const std::string& programPath) {
    std::ifstream in(programPath, std::ios::binary);
    uint32_t addr = 0;
    uint8_t byte;

    // Load program into memory
    while (in.read(reinterpret_cast<char*>(&byte), 1)) {
        mem.writeByte(addr++, byte);
    }

    if (addr == 0) {
        std::cout << "ERROR: No program loaded or file not found!" << std::endl;
        return;
    }

    std::cout << "Loaded " << addr << " program bytes:\n";
    for (uint32_t i = 0; i < addr; ++i) {
        std::cout << std::hex << std::setfill('0') << std::setw(2) << (int)mem.readByte(i) << " ";
        if ((i + 1) % 8 == 0) std::cout << std::endl; // New line every 8 bytes
    }
    std::cout << std::dec << std::endl;

    // Show first few instructions as 16-bit values
    std::cout << "\nInstructions as 16-bit values:\n";
    for (uint32_t i = 0; i < addr && i < 16; i += 2) {
        if (i + 1 < addr) {
            uint16_t instr = mem.readHalfWord(i);
            std::cout << "Addr " << i << ": 0x" << std::hex << std::setw(4) << std::setfill('0') << instr << std::dec << std::endl;
        }
    }

    int instructionCount = 0;
    const int MAX_INSTRUCTIONS = 1000; // Prevent infinite loops

    while (!halted && instructionCount < MAX_INSTRUCTIONS) {
        uint16_t instr = mem.readHalfWord(pc);
        DecodedInstr decoded = decoder.decode(instr);

        std::cout << "PC: " << pc << " | Instr: 0x" << std::hex << std::setw(4) << std::setfill('0') << instr
                  << " | Opcode: 0x" << (int)decoded.opcode << std::dec;

        // Show register values for debugging
        if (decoded.opcode == 0x0) { // ADD instruction
            std::cout << " | ADD R" << (int)decoded.rd << ", R" << (int)decoded.rs1 << ", R" << (int)decoded.rs2;
            std::cout << " | Values: " << regs[decoded.rs1] << " + " << regs[decoded.rs2] << " = " << (regs[decoded.rs1] + regs[decoded.rs2]);
        }
        std::cout << std::endl;

        // First try to handle ecalls
        halted = ecalls.tryHandle(decoded, pc);

        // If not an ecall, execute in ALU
        if (!halted) {
            alu.execute(decoded, regs, mem, pc);
        }

        pc += 2;
        instructionCount++;

        // Show register dump every 10 instructions for debugging
        if (instructionCount % 10 == 0) {
            std::cout << "\n--- Register Dump at instruction " << instructionCount << " ---" << std::endl;
            regs.dump();
            std::cout << std::endl;
        }
    }

    if (instructionCount >= MAX_INSTRUCTIONS) {
        std::cout << "Program terminated: Maximum instruction limit reached (possible infinite loop)" << std::endl;
    }

    std::cout << "Halting at PC: " << pc << " after " << instructionCount << " instructions" << std::endl;
    std::cout << "\nFinal register state:" << std::endl;
    regs.dump();
}