#include "simulator.h"
#include <fstream>
#include <iostream>
#include <iomanip>

Simulator::Simulator()
    : ecalls(mem, regs), pc(0), halted(false) {}

void Simulator::run(const std::string& programPath) {
    std::ifstream in(programPath, std::ios::binary);
    if (!in) {
        std::cerr << "ERROR: Could not open file: " << programPath << std::endl;
        return;
    }

    // Load binary program into memory
    uint32_t addr = 0;
    uint8_t byte;
    while (in.read(reinterpret_cast<char*>(&byte), 1)) {
        mem.writeByte(addr++, byte);
    }

    if (addr == 0) {
        std::cerr << "ERROR: File is empty!" << std::endl;
        return;
    }

    std::cout << "Loaded " << addr << " program bytes:\n";
    for (uint32_t i = 0; i < std::min(addr, 64u); ++i) {
        std::cout << std::hex << std::setw(2) << std::setfill('0') << (int)mem.readByte(i) << " ";
        if ((i + 1) % 8 == 0) std::cout << "\n";
    }
    std::cout << std::dec << std::endl;

    const int MAX_INSTRUCTIONS = 1000;
    int instructionCount = 0;

    while (!halted && instructionCount < MAX_INSTRUCTIONS) {
        uint16_t instr = mem.readHalfWord(pc);
        DecodedInstr decoded = decoder.decode(instr);

        std::cout << "PC: " << pc
                  << " | Instr: 0x" << std::hex << std::setw(4) << std::setfill('0') << instr
                  << " | Opcode: 0x" << (int)decoded.opcode << std::dec << std::endl;

        if (ecalls.tryHandle(decoded, pc)) {
            halted = true;  // ecall handled and requested halt (e.g., syscall 1 or 10)
            break;
        }

        // Only execute in ALU if not halted and not ecall
        alu.execute(decoded, regs, mem, pc);
        pc += 2;
        instructionCount++;
    }

    if (instructionCount >= MAX_INSTRUCTIONS) {
        std::cout << "Program terminated: Instruction limit hit (infinite loop?)" << std::endl;
    }

    std::cout << "\nFinal PC: " << pc << "\nFinal register state:\n";
    regs.dump();
}
