// ZX16 Instruction Set Simulator
// Authors: Based on project structure provided

#include <iostream>
#include <fstream>
#include <vector>
#include <cstdint>
#include <stdexcept>
#include <iomanip>

const uint32_t MEMORY_SIZE = 65536; // 64KB memory

class Memory {
public:
    Memory() : memory(MEMORY_SIZE, 0) {}

    void writeByte(uint32_t address, uint8_t value) {
        if (address >= MEMORY_SIZE) throw std::runtime_error("Write out of bounds");
        memory[address] = value;
    }

    uint8_t readByte(uint32_t address) const {
        if (address >= MEMORY_SIZE) throw std::runtime_error("Read out of bounds");
        return memory[address];
    }

    uint16_t readHalfWord(uint32_t address) const {
        if (address + 1 >= MEMORY_SIZE) throw std::runtime_error("Half-word read out of bounds");
        return (memory[address + 1] << 8) | memory[address]; // Little-endian
    }

private:
    std::vector<uint8_t> memory;
};

class CPU {
public:
    CPU(Memory &mem) : memory(mem), pc(0), halted(false) {
        registers.resize(16, 0);
    }

    void run() {
        while (!halted) {
            uint16_t instr = memory.readHalfWord(pc);
            pc += 2;
            decodeAndExecute(instr);
        }
    }

private:
    Memory &memory;
    std::vector<uint8_t> registers;
    uint16_t pc;
    bool halted;

    void decodeAndExecute(uint16_t instr) {
        uint8_t opcode = (instr >> 12) & 0xF;
        uint8_t rd = (instr >> 8) & 0xF;
        uint8_t rs = (instr >> 4) & 0xF;
        uint8_t imm = instr & 0xF;

        switch (opcode) {
            case 0xA:  // LOAD
                registers[rd] = memory.readByte(registers[rs]);
                break;
            case 0xB:  // STORE
                memory.writeByte(registers[rs], registers[rd]);
                break;
            case 0x0:  // ADD
                registers[rd] = registers[rd] + registers[rs];
                break;
            case 0xF:  // HALT
                halted = true;
                break;
            default:
                std::cerr << "Unknown opcode: " << std::hex << (int)opcode << std::endl;
                halted = true;
        }
    }
};

void loadProgram(Memory &mem, const std::string &filename) {
    std::ifstream infile(filename, std::ios::binary);
    if (!infile) throw std::runtime_error("Cannot open input file");

    uint8_t byte;
    uint32_t addr = 0;
    while (infile.read((char*)&byte, 1)) {
        mem.writeByte(addr++, byte);
    }
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: zx16sim <program.bin>" << std::endl;
        return 1;
    }

    try {
        Memory mem;
        loadProgram(mem, argv[1]);
        CPU cpu(mem);
        cpu.run();
    } catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
