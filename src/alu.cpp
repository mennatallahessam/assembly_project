// #include "alu.h"
// #include <stdexcept>
// #include <iostream>
// #include <cstdlib>   // For std::exit
//
// void ALU::handleSysCall(int syscallCode, Registers& regs, Memory& mem) {
//     switch (syscallCode) {
//         case 1:  // Print integer in x6 (a0)
//             std::cout << regs[6] << std::endl;
//         break;
//
//         case 2: {  // Print string from address in x11 (r11)
//             uint16_t addr = regs[11];
//             char ch;
//             while ((ch = mem.readByte(addr++)) != 0) {
//                 std::cout << ch;
//             }
//             break;
//         }
//
//         case 10: // Exit program
//             std::exit(0);
//         break;
//
//         default:
//             std::cerr << syscallCode << std::endl;
//         break;
//     }
// }
//
//
// void ALU::execute(const DecodedInstr& instr, Registers& regs, Memory& mem, uint16_t& pc) {
//    // std::cout << "ALU executing opcode: 0x" << std::hex << (int)instr.opcode << std::dec << std::endl;
//
//     switch (instr.opcode) {
//         case 0x0: // ADD
//             regs[instr.rd] = regs[instr.rs1] + regs[instr.rs2];
//             break;
//
//         case 0x1: // LOAD_IMM (e.g., LI)
//             regs[instr.rd] = instr.immediate;
//             break;
//
//         case 0x2: // SUB
//             regs[instr.rd] = regs[instr.rs1] - regs[instr.rs2];
//             break;
//
//         case 0x3: // AND
//             regs[instr.rd] = regs[instr.rs1] & regs[instr.rs2];
//             break;
//
//         case 0x4: // OR
//             regs[instr.rd] = regs[instr.rs1] | regs[instr.rs2];
//             break;
//
//         case 0x5: // XOR
//             regs[instr.rd] = regs[instr.rs1] ^ regs[instr.rs2];
//             break;
//
//         case 0x6: // ADDI (corrected to use rs1, not rd)
//             regs[instr.rd] = regs[instr.rs1] + instr.immediate;
//             break;
//
//         case 0x7: // SHL (logical left shift)
//             regs[instr.rd] = regs[instr.rs1] << instr.immediate;
//             break;
//
//         case 0x8: // SHR (logical right shift)
//             regs[instr.rd] = regs[instr.rs1] >> instr.immediate;
//             break;
//
//         case 0x9: // MUL
//             regs[instr.rd] = regs[instr.rs1] * regs[instr.rs2];
//             break;
//
//         case 0xA: // DIV
//             if (regs[instr.rs2] == 0)
//                 throw std::runtime_error("Division by zero");
//             regs[instr.rd] = regs[instr.rs1] / regs[instr.rs2];
//             break;
//
//         case 0xB: // LOAD (e.g., LW)
//         {
//             uint16_t addr = regs[instr.rs1] + instr.immediate;
//             regs[instr.rd] = mem.readHalfWord(addr);
//             break;
//         }
//
//         case 0xC: // STORE (e.g., SW)
//         {
//             uint16_t addr = regs[instr.rs1] + instr.immediate;
//             mem.writeHalfWord(addr, regs[instr.rd]);
//             break;
//         }
//
//         case 0xD: // BEQ
//             if (regs[instr.rs1] == regs[instr.rs2]) {
//                 pc += instr.immediate - 2;  // PC already increments by 2 after exec
//             }
//             break;
//
//         case 0xE: // JAL
//             regs[instr.rd] = pc + 2;
//             pc += instr.immediate - 2;
//             break;
//
//         case 0xF: { // ECALL
//             int syscallCode = regs[10]; // r10
//             this->handleSysCall(syscallCode, regs, mem);
//
//    break;
//         }
//
//         default:
//            // std::cerr << "ALU: Unsupported opcode 0x" << std::hex << (int)instr.opcode << std::dec << std::endl;
//             break;
//     }
// }
