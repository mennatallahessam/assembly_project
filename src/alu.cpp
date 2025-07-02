#include "ALU.h"
#include <iostream>
#include <cassert>

void ALU::execute(const DecodedInstruction& instr, Registers& regs, Memory& mem, uint32_t& pc, bool& halted) {
    switch (instr.format) {
        case FORMAT_R:     executeRType(instr, regs); break;
        case FORMAT_I:     executeIType(instr, regs, pc); break;
        case FORMAT_L:     executeLoad(instr, regs, mem); break;
        case FORMAT_S:     executeStore(instr, regs, mem); break;
        case FORMAT_B:     executeBranch(instr, regs, pc); break;
        case FORMAT_J:     executeJAL(instr, regs, pc); break;
        case FORMAT_IJ:    executeJALR(instr, regs, pc); break;
        case FORMAT_U:     
            if (instr.mnemonic == "LUI") executeLUI(instr, regs);
            else if (instr.mnemonic == "AUIPC") executeAUIPC(instr, regs, pc);
            break;
        case FORMAT_SYS:   executeECALL(regs, mem, halted); break;
        default:           std::cerr << "Illegal instruction!\n"; halted = true; break;
    }
}

void ALU::executeRType(const DecodedInstruction& instr, Registers& regs) {
    uint32_t rs1 = regs[instr.rs1];
    uint32_t rs2 = regs[instr.rs2];
    uint32_t result = 0;
    if (instr.mnemonic == "ADD")      result = rs1 + rs2;
    else if (instr.mnemonic == "SUB") result = rs1 - rs2;
    else if (instr.mnemonic == "SLL") result = rs1 << (rs2 & 0x1F);
    else if (instr.mnemonic == "SLT") result = (int32_t)rs1 < (int32_t)rs2;
    else if (instr.mnemonic == "SLTU")result = rs1 < rs2;
    else if (instr.mnemonic == "XOR") result = rs1 ^ rs2;
    else if (instr.mnemonic == "SRL") result = rs1 >> (rs2 & 0x1F);
    else if (instr.mnemonic == "SRA") result = (int32_t)rs1 >> (rs2 & 0x1F);
    else if (instr.mnemonic == "OR")  result = rs1 | rs2;
    else if (instr.mnemonic == "AND") result = rs1 & rs2;
    regs[instr.rd] = result;
}

void ALU::executeIType(const DecodedInstruction& instr, Registers& regs, uint32_t& pc) {
    uint32_t rs1 = regs[instr.rs1];
    int32_t imm = instr.imm;
    uint32_t result = 0;
    if (instr.mnemonic == "ADDI")      result = rs1 + imm;
    else if (instr.mnemonic == "SLTI") result = (int32_t)rs1 < imm;
    else if (instr.mnemonic == "SLTIU")result = rs1 < (uint32_t)imm;
    else if (instr.mnemonic == "XORI") result = rs1 ^ imm;
    else if (instr.mnemonic == "ORI")  result = rs1 | imm;
    else if (instr.mnemonic == "ANDI") result = rs1 & imm;
    else if (instr.mnemonic == "SLLI") result = rs1 << (imm & 0x1F);
    else if (instr.mnemonic == "SRLI") result = rs1 >> (imm & 0x1F);
    else if (instr.mnemonic == "SRAI") result = (int32_t)rs1 >> (imm & 0x1F);
    else if (instr.mnemonic == "JALR") { executeJALR(instr, regs, pc); return; }
    regs[instr.rd] = result;
}

void ALU::executeLoad(const DecodedInstruction& instr, Registers& regs, Memory& mem) {
    uint32_t addr = regs[instr.rs1] + instr.imm;
    if (instr.mnemonic == "LB") {
        int8_t val = mem.readByte(addr);
        regs[instr.rd] = int32_t(val);
    } else if (instr.mnemonic == "LH") {
        int16_t val = mem.readHalfWord(addr);
        regs[instr.rd] = int32_t(val);
    } else if (instr.mnemonic == "LW") {
        regs[instr.rd] = mem.readWord(addr);
    } else if (instr.mnemonic == "LBU") {
        regs[instr.rd] = mem.readByte(addr);
    } else if (instr.mnemonic == "LHU") {
        regs[instr.rd] = mem.readHalfWord(addr);
    }
}

void ALU::executeStore(const DecodedInstruction& instr, Registers& regs, Memory& mem) {
    uint32_t addr = regs[instr.rs1] + instr.imm;
    uint32_t val = regs[instr.rs2];
    if (instr.mnemonic == "SB") {
        mem.writeByte(addr, val & 0xFF);
    } else if (instr.mnemonic == "SH") {
        mem.writeHalfWord(addr, val & 0xFFFF);
    } else if (instr.mnemonic == "SW") {
        mem.writeWord(addr, val);
    }
}

void ALU::executeBranch(const DecodedInstruction& instr, Registers& regs, uint32_t& pc) {
    uint32_t rs1 = regs[instr.rs1];
    uint32_t rs2 = regs[instr.rs2];
    bool take = false;
    if (instr.mnemonic == "BEQ")      take = (rs1 == rs2);
    else if (instr.mnemonic == "BNE") take = (rs1 != rs2);
    else if (instr.mnemonic == "BLT") take = ((int32_t)rs1 < (int32_t)rs2);
    else if (instr.mnemonic == "BGE") take = ((int32_t)rs1 >= (int32_t)rs2);
    else if (instr.mnemonic == "BLTU")take = (rs1 < rs2);
    else if (instr.mnemonic == "BGEU")take = (rs1 >= rs2);
    if (take) pc += instr.imm - 4; // -4 because PC will be incremented by 4 after this
}

void ALU::executeJAL(const DecodedInstruction& instr, Registers& regs, uint32_t& pc) {
    regs[instr.rd] = pc + 4;
    pc += instr.imm - 4;
}

void ALU::executeJALR(const DecodedInstruction& instr, Registers& regs, uint32_t& pc) {
    uint32_t t = pc + 4;
    pc = (regs[instr.rs1] + instr.imm) & ~1;
    regs[instr.rd] = t;
    pc -= 4; // Because PC will be incremented by 4 after this
}

void ALU::executeLUI(const DecodedInstruction& instr, Registers& regs) {
    regs[instr.rd] = instr.imm << 12;
}

void ALU::executeAUIPC(const DecodedInstruction& instr, Registers& regs, uint32_t pc) {
    regs[instr.rd] = pc + (instr.imm << 12);
}

void ALU::executeECALL(Registers& regs, Memory& mem, bool& halted) {
    uint32_t syscall = regs[17]; // a7
    if (syscall == 1) { // print int
        std::cout << (int32_t)regs[10]; // a0
    } else if (syscall == 4) { // print string
        uint32_t addr = regs[10];
        char c;
        while ((c = (char)mem.readByte(addr++)) != '\0')
            std::cout << c;
    } else if (syscall == 10) { // exit
        halted = true;
    } else {
        std::cerr << "Unknown ECALL: " << syscall << std::endl;
        halted = true;
    }
}
