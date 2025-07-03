#include "alu.h"
#include <iostream>

// Helper for sign extension
static int16_t sign_extend(uint16_t val, int bits) {
    int16_t mask = 1 << (bits - 1);
    return (val ^ mask) - mask;
}

void ALU::execute(const DecodedInstruction& instr, Registers& regs, Memory& mem, uint16_t& pc, bool& halted, Ecalls& ecalls, graphics& gfx) {
    const std::string& m = instr.mnemonic;
    // Arithmetic
    if (m == "ADD"){
        regs[instr.rd] = regs[instr.rs1] + regs[instr.rs2];
}
    else if (m == "SUB") {
            regs[instr.rd] = regs[instr.rs1] - regs[instr.rs2];
    } else if (m == "AND") {
            regs[instr.rd] = regs[instr.rs1] & regs[instr.rs2];
    } else if (m == "OR") {
            regs[instr.rd] = regs[instr.rs1] | regs[instr.rs2];
    } else if (m == "XOR") {
            regs[instr.rd] = regs[instr.rs1] ^ regs[instr.rs2];
    } else if (m == "SLL") {
            regs[instr.rd] = regs[instr.rs1] << (regs[instr.rs2] & 0xF);
    } else if (m == "SRL") {
            regs[instr.rd] = regs[instr.rs1] >> (regs[instr.rs2] & 0xF);
    } else if (m == "SRA") {
            regs[instr.rd] = int16_t(regs[instr.rs1]) >> (regs[instr.rs2] & 0xF);
    }

    else if (m == "ADDI") {
            regs[instr.rd] = regs[instr.rs1] + instr.imm;  // imm is already sign-extended
    } else if (m == "ANDI") {
            regs[instr.rd] = regs[instr.rs1] & instr.imm;
    } else if (m == "ORI") {
            regs[instr.rd] = regs[instr.rs1] | instr.imm;
    } else if (m == "XORI") {
            regs[instr.rd] = regs[instr.rs1] ^ instr.imm;
    } else if (m == "SLLI") {
            regs[instr.rd] = regs[instr.rs1] << (instr.imm & 0xF);
    } else if (m == "SRLI") {
            regs[instr.rd] = regs[instr.rs1] >> (instr.imm & 0xF);
    } else if (m == "SRAI") {
            regs[instr.rd] = int16_t(regs[instr.rs1]) >> (instr.imm & 0xF);
    }

        // Load/Store (L-Type/S-Type)
    else if (m == "LW") {
            regs[instr.rd] = mem.readHalfWord(regs[instr.rs2] + instr.imm);  // ZX16 uses 16-bit words
    } else if (m == "LB") {
            regs[instr.rd] = mem.readByte(regs[instr.rs2] + instr.imm);
    } else if (m == "LBU") {
            regs[instr.rd] = mem.readByte(regs[instr.rs2] + instr.imm) & 0xFF;  // Zero-extend byte
    } else if (m == "SW") {
            mem.writeHalfWord(regs[instr.rs1] + instr.imm, regs[instr.rs2]);
    } else if (m == "SB") {
            mem.writeByte(regs[instr.rs1] + instr.imm, regs[instr.rs2] & 0xFF);
    }

        // Branches (B-Type)
    else if (m == "BEQ") {
            if (regs[instr.rs1] == regs[instr.rs2])
                    pc += instr.imm;
    } else if (m == "BNE") {
            if (regs[instr.rs1] != regs[instr.rs2])
                    pc += instr.imm;
    } else if (m == "BZ") {
            if (regs[instr.rs1] == 0)
                    pc += instr.imm;
    } else if (m == "BNZ") {
            if (regs[instr.rs1] != 0)
                    pc += instr.imm;
    } else if (m == "BLT") {
            if (int16_t(regs[instr.rs1]) < int16_t(regs[instr.rs2]))
                    pc += instr.imm;
    } else if (m == "BGE") {
            if (int16_t(regs[instr.rs1]) >= int16_t(regs[instr.rs2]))
                    pc += instr.imm;
    } else if (m == "BLTU") {
            if (regs[instr.rs1] < regs[instr.rs2])
                    pc += instr.imm;
    } else if (m == "BGEU") {
            if (regs[instr.rs1] >= regs[instr.rs2])
                    pc += instr.imm;
    }

        // Jumps (J-Type)
    else if (m == "JAL") {
            regs[instr.rd] = pc + 2;  // Save return address
            pc += instr.imm;
    } else if (m == "J") {
            pc += instr.imm;
    } else if (m == "JALR") {
            uint16_t t = pc + 2;
            pc = (regs[instr.rs1] + instr.imm) & ~1;  // Clear LSB for alignment
            regs[instr.rd] = t;
    }
        // Upper (U-Type)
    else if (m == "LUI") {
            regs[instr.rd] = instr.imm;  // Load upper immediate
    } else if (m == "AUIPC") {
            regs[instr.rd] = pc + instr.imm;  // Add upper immediate to PC
    }

        // System (SYS-Type)
    else if (m == "ECALL") {
            ecalls.handle(instr, regs, mem, halted, gfx);  // Pass the instruction to ecalls
    }

        // Unknown
    else {
            std::cerr << "Unknown opcode: " << instr.mnemonic << std::endl;
            halted = true;
    }
}