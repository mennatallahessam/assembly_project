#include "alu.h"
#include <stdexcept>
#include <iostream>

void ALU::execute(const DecodedInstr& instr, Registers& regs) {
    std::cout << "ALU executing opcode: 0x" << std::hex << (int)instr.opcode << std::dec << std::endl;

    switch (instr.opcode) {
        case 0x0: // ADD (register-register)
            regs[instr.rd] = regs[instr.rs1] + regs[instr.rs2];
        break;

        case 0x1: // LOAD_IMM (load immediate)
            regs[instr.rd] = instr.immediate;
        break;

        case 0x2: // SUB (register-register)
            regs[instr.rd] = regs[instr.rs1] - regs[instr.rs2];
        break;

        case 0x3: // AND (register-register)
            regs[instr.rd] = regs[instr.rs1] & regs[instr.rs2];
        break;

        case 0x4: // OR (register-register)
            regs[instr.rd] = regs[instr.rs1] | regs[instr.rs2];
        break;

        case 0x5: // XOR (register-register)
            regs[instr.rd] = regs[instr.rs1] ^ regs[instr.rs2];
        break;

        case 0x6: // ADD_IMM (add immediate)
            regs[instr.rd] = regs[instr.rd] + instr.immediate;
        break;

        case 0x7: // SHL (shift left immediate)
            regs[instr.rd] = regs[instr.rs1] << instr.immediate;
        break;

        case 0x8: // SHR (shift right immediate)
            regs[instr.rd] = regs[instr.rs1] >> instr.immediate;
        break;

        case 0x9: // MUL (register-register)
            regs[instr.rd] = regs[instr.rs1] * regs[instr.rs2];
        break;

        case 0xA: // DIV (register-register)
            if (regs[instr.rs2] == 0)
                throw std::runtime_error("Division by zero");
        regs[instr.rd] = regs[instr.rs1] / regs[instr.rs2];
        break;

        default:
            // Unknown or unsupported opcode
                std::cerr << "ALU: Unsupported opcode 0x" << std::hex << (int)instr.opcode << std::dec << std::endl;
        break;
    }
}
