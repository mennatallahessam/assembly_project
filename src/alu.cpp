#include "alu.h"
#include <iostream>

static int16_t sign_extend(uint16_t val, int bits) {
    int16_t mask = 1 << (bits - 1);
    return (val ^ mask) - mask;
}

void ALU::execute(const DecodedInstruction& d, Registers& regs, Memory& mem, uint16_t& pc, bool& halted, Ecalls& ecalls, Graphics& gfx) {
    const std::string& m = d.mnemonic;

    switch (d.format) {
        case FORMAT_R: {
            uint16_t rs1_val = regs.get(d.rs1);
            uint16_t rs2_val = regs.get(d.rs2);
            uint16_t result = 0;

            if (m == "ADD") result = rs1_val + rs2_val;
            else if (m == "SUB") result = rs1_val - rs2_val;
            else if (m == "AND") result = rs1_val & rs2_val;
            else if (m == "OR")  result = rs1_val | rs2_val;
            else if (m == "XOR") result = rs1_val ^ rs2_val;
            else if (m == "SLT") result = int16_t(rs1_val) < int16_t(rs2_val) ? 1 : 0;
            else if (m == "SLTU") result = rs1_val < rs2_val ? 1 : 0;
            else if (m == "SLL") result = rs1_val << (rs2_val & 0xF);
            else if (m == "SRL") result = rs1_val >> (rs2_val & 0xF);
            else if (m == "SRA") result = int16_t(rs1_val) >> (rs2_val & 0xF);
            else if (m == "MV")  result = rs2_val;
            else if (m == "JR") {
                pc = regs.get(d.rd);
                return;
            }
            else if (m == "JALR") {
                uint16_t ret_addr = pc;
                pc = regs.get(d.rs2);
                regs.set(d.rd, ret_addr);
                return;
            }

            regs.set(d.rd, result);
            break;
        }

        case FORMAT_I: {
            uint16_t rs1_val = regs.get(d.rs1);
            int16_t imm = d.imm;
            uint16_t result = 0;

            if (m == "ADDI") result = rs1_val + imm;
            else if (m == "LI") result = imm;
            else if (m == "SLTI") result = int16_t(rs1_val) < imm ? 1 : 0;
            else if (m == "SLTUI") result = rs1_val < uint16_t(imm) ? 1 : 0;
            else if (m == "SLLI") result = rs1_val << (imm & 0xF);
            else if (m == "SRLI") result = rs1_val >> (imm & 0xF);
            else if (m == "SRAI") result = int16_t(rs1_val) >> (imm & 0xF);
            else if (m == "ORI") result = rs1_val | imm;
            else if (m == "ANDI") result = rs1_val & imm;
            else if (m == "XORI") result = rs1_val ^ imm;

            regs.set(d.rd, result);
            break;
        }

        case FORMAT_S: {
            uint16_t addr = regs.get(d.rs1) + d.imm;
            if (m == "SW") {
                if (addr & 1) {
                    std::cerr << "Unaligned SW at " << std::hex << addr << "\n";
                    halted = true;
                    return;
                }
                mem.store16(addr, regs.get(d.rs2));
            } else if (m == "SB") {
                if (d.format == FORMAT_S && d.s_op == STOP_SB) {
                    uint16_t addr = regs.get(d.rs1) + d.imm;
                    uint8_t value = regs.get(d.rs2) & 0xFF;
                    std::cout << "[SB] Writing " << std::hex << (int)value << " to 0x" << addr << std::endl;
                }

                uint8_t val = regs.get(d.rs2) & 0xFF;
               // std::cout << "[SB] Writing " << +val << " to address " << std::hex << addr << "\n";
                mem.store8(addr, val);
            }

            break;
        }

        case FORMAT_L: {
            uint16_t addr = regs.get(d.rs2) + d.imm;
            if (m == "LW") {
                if (addr & 1) {
                    std::cerr << "Unaligned LW at " << std::hex << addr << "\n";
                    halted = true;
                    return;
                }
                regs.set(d.rd, mem.load16(addr));
            } else if (m == "LB") {
                regs.set(d.rd, sign_extend(mem.load8(addr), 8));
            } else if (m == "LBU") {
                regs.set(d.rd, mem.load8(addr));
            }
            break;
        }

        case FORMAT_B: {
            bool cond = false;
            uint16_t rs1_val = regs.get(d.rs1);
            uint16_t rs2_val = regs.get(d.rs2);

            if (m == "BEQ") cond = rs1_val == rs2_val;
            else if (m == "BNE") cond = rs1_val != rs2_val;
            else if (m == "BZ")  cond = rs1_val == 0;  // Only check rs1, ignore rs2
            else if (m == "BNZ") cond = rs1_val != 0; // Only check rs1, ignore rs2
            else if (m == "BLT") cond = int16_t(rs1_val) < int16_t(rs2_val);
            else if (m == "BGE") cond = int16_t(rs1_val) >= int16_t(rs2_val);
            else if (m == "BLTU") cond = rs1_val < rs2_val;
            else if (m == "BGEU") cond = rs1_val >= rs2_val;

            if (cond) {
                // Calculate target from instruction address (PC was already incremented)
                pc = (pc ) + d.imm;
                return; // Don't increment PC again
            }
            // If condition false, PC increment in main loop handles it
            break;
        }

        case FORMAT_J: {
            if (d.rd != 0) { // JAL
                regs.set(d.rd, pc); // Save return address (already incremented PC)
            }
            // Calculate jump target from instruction address
            pc = (pc - 2) + d.imm;
            return; // Don't increment PC again
        }

        case FORMAT_U: {
            if (m == "LUI") {
                regs.set(d.rd, d.imm);
            } else if (m == "AUIPC") {
                // Use the PC of the instruction, not the incremented PC
                uint16_t instruction_pc = pc - 2;  // Subtract 2 to get instruction address
                regs.set(d.rd, instruction_pc + d.imm);
            }
            break;
        }
        case FORMAT_SYS: {
            // ECALL should be handled in main, not here
            std::cerr << "ECALL should be handled in main, not in ALU" << std::endl;
            halted = true;
            break;
        }

        default:
            std::cerr << "Unknown format for instruction " << m << std::endl;
            halted = true;
            break;
    }
}
