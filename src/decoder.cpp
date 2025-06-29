#include "decoder.h"

DecodedInstr decoder::decode(uint16_t instr) {
    DecodedInstr decoded;

    decoded.opcode = (instr >> 12) & 0xF;  // bits 15-12
    decoded.rd     = (instr >> 8)  & 0xF;  // bits 11-8

    switch (decoded.opcode) {
        case 0x0: // Example: ADD (register-register)
        case 0x2: // Example: SUB
        case 0x3: // Example: AND
        case 0x4: // Example: OR
        case 0x5: // Example: XOR
            // Register-register format
            decoded.rs1 = (instr >> 4) & 0xF;  // bits 7-4
        decoded.rs2 = instr & 0xF;         // bits 3-0
        decoded.immediate = 0;
        break;

        case 0x1: // LOAD_IMM (immediate format)
        case 0x6: // Example: ADD_IMM
            // Immediate in lower 8 bits (7-0)
            decoded.immediate = static_cast<int8_t>(instr & 0xFF); // Sign extend 8-bit immediate
        decoded.rs1 = 0xF;  // No valid register
        decoded.rs2 = 0xF;  // No valid register
        break;

        case 0xA: // LOAD from memory (e.g. LOAD [rs1] → rd)
        case 0xB: // STORE to memory (e.g. STORE rd → [rs1])
            // Could have only rs1 and rd, rs2 unused
            decoded.rs1 = (instr >> 4) & 0xF;
        decoded.rs2 = 0xF;  // unused
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
