#include "alu.h"
#include <iostream>

// Helper for sign extension
static int16_t sign_extend(uint16_t val, int bits) {
    int16_t mask = 1 << (bits - 1);
    return (val ^ mask) - mask;
}

void ALU::execute(const DecodedInstruction& instr, Registers& regs, Memory& mem, uint16_t& pc, bool& halted, Ecalls& ecalls, Graphics& gfx) {
    switch (instr.opcode) {
        // Arithmetic
        case ZX16_OP_ADD:
            regs[instr.rd] = regs[instr.rs1] + regs[instr.rs2];
            break;
        case ZX16_OP_SUB:
            regs[instr.rd] = regs[instr.rs1] - regs[instr.rs2];
            break;
        case ZX16_OP_AND:
            regs[instr.rd] = regs[instr.rs1] & regs[instr.rs2];
            break;
        case ZX16_OP_OR:
            regs[instr.rd] = regs[instr.rs1] | regs[instr.rs2];
            break;
        case ZX16_OP_XOR:
            regs[instr.rd] = regs[instr.rs1] ^ regs[instr.rs2];
            break;
        case ZX16_OP_SLL:
            regs[instr.rd] = regs[instr.rs1] << (regs[instr.rs2] & 0xF);
            break;
        case ZX16_OP_SRL:
            regs[instr.rd] = regs[instr.rs1] >> (regs[instr.rs2] & 0xF);
            break;
        case ZX16_OP_SRA:
            regs[instr.rd] = int16_t(regs[instr.rs1]) >> (regs[instr.rs2] & 0xF);
            break;
        // Immediate
        case ZX16_OP_ADDI:
            regs[instr.rd] = regs[instr.rs1] + sign_extend(instr.imm, instr.imm_bits);
            break;
        case ZX16_OP_ANDI:
            regs[instr.rd] = regs[instr.rs1] & sign_extend(instr.imm, instr.imm_bits);
            break;
        case ZX16_OP_ORI:
            regs[instr.rd] = regs[instr.rs1] | sign_extend(instr.imm, instr.imm_bits);
            break;
        case ZX16_OP_XORI:
            regs[instr.rd] = regs[instr.rs1] ^ sign_extend(instr.imm, instr.imm_bits);
            break;
        case ZX16_OP_SLLI:
            regs[instr.rd] = regs[instr.rs1] << (instr.imm & 0xF);
            break;
        case ZX16_OP_SRLI:
            regs[instr.rd] = regs[instr.rs1] >> (instr.imm & 0xF);
            break;
        case ZX16_OP_SRAI:
            regs[instr.rd] = int16_t(regs[instr.rs1]) >> (instr.imm & 0xF);
            break;
        // Load/Store
        case ZX16_OP_LW:
            regs[instr.rd] = mem.readWord(regs[instr.rs1] + sign_extend(instr.imm, instr.imm_bits));
            break;
        case ZX16_OP_SW:
            mem.writeWord(regs[instr.rs1] + sign_extend(instr.imm, instr.imm_bits), regs[instr.rs2]);
            break;
        // Branches
        case ZX16_OP_BEQ:
            if (regs[instr.rs1] == regs[instr.rs2])
                pc += sign_extend(instr.imm, instr.imm_bits);
            break;
        case ZX16_OP_BNE:
            if (regs[instr.rs1] != regs[instr.rs2])
                pc += sign_extend(instr.imm, instr.imm_bits);
            break;
        // Jumps
        case ZX16_OP_JAL:
            regs[instr.rd] = pc + 2;
            pc += sign_extend(instr.imm, instr.imm_bits);
            break;
        case ZX16_OP_JALR: {
            uint16_t t = pc + 2;
            pc = (regs[instr.rs1] + sign_extend(instr.imm, instr.imm_bits)) & ~1;
            regs[instr.rd] = t;
            break;
        }
        // LUI
        case ZX16_OP_LUI:
            regs[instr.rd] = instr.imm << 4; // Adjust shift as needed
            break;
        // System
        case ZX16_OP_ECALL:
            ecalls.handle(regs, mem, halted, gfx);
            break;
        default:
            std::cerr << "Unknown opcode: " << instr.opcode << std::endl;
            halted = true;
    }
}
