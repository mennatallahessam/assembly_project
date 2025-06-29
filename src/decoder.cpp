#include "decoder.h"
DecodedInstr decoder::decode(uint16_t instr) {
    DecodedInstr decoded;

    decoded.opcode = (instr >> 12) & 0xF;  // bits 15-12
    decoded.rd     = (instr >> 8)  & 0xF;  // bits 11-8

    switch (decoded.opcode) {
        case 0x0: // ADD (register-register)
        case 0x2: // SUB
        case 0x3: // AND
        case 0x4: // OR
        case 0x5: // XOR
        case 0x9: // MUL
            // Register-register format
            decoded.rs1 = (instr >> 4) & 0xF;  // bits 7-4
            decoded.rs2 = instr & 0xF;         // bits 3-0
            decoded.immediate = 0;
            break;

        case 0x1: // LOAD_IMM (immediate format)
        case 0x6: // ADD_IMM
            // Immediate in lower 8 bits (7-0)
            decoded.immediate = static_cast<int8_t>(instr & 0xFF); // Sign extend 8-bit immediate
            decoded.rs1 = 0xF;  // No valid register
            decoded.rs2 = 0xF;  // No valid register
            break;

        case 0x7: // SHL (shift left immediate)
        case 0x8: // SHR (shift right immediate)
            // Shift format
            decoded.rs1 = (instr >> 4) & 0xF;
            decoded.immediate = instr & 0xF;  // 4-bit shift amount
            decoded.rs2 = 0xF;  // unused
            break;

        case 0xA: // DIV (register-register) - keep existing
            decoded.rs1 = (instr >> 4) & 0xF;
            decoded.rs2 = instr & 0xF;
            decoded.immediate = 0;
            break;

        case 0xB: // LOAD from memory (LW)
            // Load format: rd = mem[rs1 + immediate]
            decoded.rs1 = (instr >> 4) & 0xF;  // Base address register
            decoded.immediate = static_cast<int8_t>(instr & 0xF); // 4-bit offset
            if (decoded.immediate & 0x8) decoded.immediate |= 0xFFF0; // Sign extend to 16-bit
            decoded.rs2 = 0xF;  // unused
            break;

        case 0xC: // STORE to memory (SW)
            // Store format: mem[rs1 + immediate] = rd
            decoded.rs1 = (instr >> 4) & 0xF;  // Base address register
            decoded.immediate = static_cast<int8_t>(instr & 0xF); // 4-bit offset
            if (decoded.immediate & 0x8) decoded.immediate |= 0xFFF0; // Sign extend to 16-bit
            decoded.rs2 = 0xF;  // unused
            break;

        case 0xD: // Branch Equal (BEQ)
            // Branch format: if (rs1 == rs2) pc += immediate
            decoded.rs1 = (instr >> 4) & 0xF;  // First source register
            decoded.rs2 = instr & 0xF;         // Second source register
            decoded.immediate = static_cast<int8_t>((instr >> 0) & 0xF); // Use lower 4 bits for branch offset
            if (decoded.immediate & 0x8) decoded.immediate |= 0xFFF0; // Sign extend
            break;

        case 0xE: // Jump and Link (JAL)
            // Jump format: rd = pc + 2; pc += immediate
            decoded.immediate = static_cast<int8_t>(instr & 0xFF); // 8-bit jump offset
            decoded.rs1 = 0xF;
            decoded.rs2 = 0xF;
            break;

        case 0xF: // ECALL/SYSTEM - handled by ecalls
            decoded.rs1 = 0xF;
            decoded.rs2 = 0xF;
            decoded.immediate = 0;
            break;

        default:
            // Unknown opcode - default to zeros
            decoded.rs1 = 0xF;
            decoded.rs2 = 0xF;
            decoded.immediate = 0;
            break;
    }

    return decoded;
}