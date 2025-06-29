#include "simulator.h"
#include <fstream>
#include <iostream>

Simulator::Simulator()
    : ecalls(mem, regs), pc(0), halted(false) {}

void Simulator::run(const std::string& programPath) {
    std::ifstream in(programPath, std::ios::binary);
    uint32_t addr = 0;
    uint8_t byte;

    while (in.read(reinterpret_cast<char*>(&byte), 1)) {
        mem.writeByte(addr++, byte);
    }

    std::cout << "Loaded program bytes:\n";
    for (uint32_t i = 0; i < addr; ++i) {
        std::cout << std::hex << (int)mem.readByte(i) << " ";
    }
    std::cout << std::dec << std::endl;

    while (!halted) {
        uint16_t instr = mem.readHalfWord(pc);
        DecodedInstr decoded = decoder.decode(instr);

        std::cout << "PC: " << pc << " | Opcode: 0x" << std::hex << (int)decoded.opcode << std::dec << std::endl;

        halted = ecalls.tryHandle(decoded, pc);

        if (!halted && decoded.opcode != 0xF) {
            alu.execute(decoded, regs);
        }

        pc += 2;
    }

    std::cout << "Halting at PC: " << pc << std::endl;
}
