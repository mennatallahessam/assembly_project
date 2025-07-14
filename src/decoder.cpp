#include "Decoder.h"
#include "utils.h"
#include <iostream>
#include <iomanip>
using namespace std;

InstructionFormat Decoder::getInstructionFormat(uint16_t inst) {
    uint8_t opcode = inst & 0x7; // bits [2:0]
    switch (opcode) {
        case 0b000: return FORMAT_R;
        case 0b001: return FORMAT_I;
        case 0b010: return FORMAT_B;
        case 0b011: return FORMAT_S;
        case 0b100: return FORMAT_L;
        case 0b101: return FORMAT_J;
        case 0b110: return FORMAT_U;
        case 0b111: return FORMAT_SYS;
        default:    return FORMAT_UNKNOWN;
    }
}

RTypeOp Decoder::decodeRTypeOperation(uint16_t inst) {
    // Extract function fields according to ZX16 R-type format
    uint8_t funct4 = (inst >> 12) & 0xF;  // bits [15:12]
    uint8_t func3  = (inst >> 3)  & 0x7;  // bits [5:3]
    uint8_t key = (funct4 << 3) | func3;

    switch (key) {
        case 0b0000000: return RTOP_ADD;
        case 0b0001000: return RTOP_SUB;
        case 0b0010001: return RTOP_SLT;
        case 0b0011010: return RTOP_SLTU;
        case 0b0100011: return RTOP_SLL;
        case 0b0101011: return RTOP_SRL;
        case 0b0110011: return RTOP_SRA;
        case 0b0111100: return RTOP_OR;
        case 0b1000101: return RTOP_AND;
        case 0b1001110: return RTOP_XOR;
        case 0b1010111: return RTOP_MV;
        case 0b1011000: return RTOP_JR;
        case 0b1100000: return RTOP_JALR;
        default:        return RTOP_UNKNOWN;
    }
}

std::string Decoder::rTypeOpToString(RTypeOp op) {
    switch (op) {
        case RTOP_ADD: return "ADD";
        case RTOP_SUB: return "SUB";
        case RTOP_SLT: return "SLT";
        case RTOP_SLTU: return "SLTU";
        case RTOP_SLL: return "SLL";
        case RTOP_SRL: return "SRL";
        case RTOP_SRA: return "SRA";
        case RTOP_OR:  return "OR";
        case RTOP_AND: return "AND";
        case RTOP_XOR: return "XOR";
        case RTOP_MV:  return "MV";
        case RTOP_JR:  return "JR";
        case RTOP_JALR:return "JALR";
        default:       return "UNKNOWN_R";
    }
}

ITypeOp Decoder::decodeITypeOperation(uint16_t inst) {
    uint8_t imm7 = (inst >> 9) & 0x7F;         // imm[6:0]
    uint8_t imm_shift_type = (imm7 >> 4) & 0x7; // imm[6:4]
    uint8_t funct3 = (inst >> 3) & 0x7;

    switch (funct3) {
        case 0b000: return ITOP_ADDI;
        case 0b001: return ITOP_SLTI;
        case 0b010: return ITOP_SLTUI;
        case 0b011:
            if (imm_shift_type == 0b010)
                return ITOP_SRLI;
            else if (imm_shift_type == 0b100)
                return ITOP_SRAI;
            else
                return ITOP_SLLI;
        case 0b100: return ITOP_ORI;
        case 0b101: return ITOP_ANDI;
        case 0b110: return ITOP_XORI;
        case 0b111: return ITOP_LI;
        default:    return ITOP_UNKNOWN;
    }
}

std::string Decoder::iTypeOpToString(ITypeOp op) {
    switch (op) {
        case ITOP_ADDI:  return "ADDI";
        case ITOP_SLTI:  return "SLTI";
        case ITOP_SLTUI: return "SLTUI";
        case ITOP_SLLI:  return "SLLI";
        case ITOP_SRLI:  return "SRLI";
        case ITOP_SRAI:  return "SRAI";
        case ITOP_ORI:   return "ORI";
        case ITOP_ANDI:  return "ANDI";
        case ITOP_XORI:  return "XORI";
        case ITOP_LI:    return "LI";
        default:         return "UNKNOWN_I";
    }
}

BTypeOp Decoder::decodeBTypeOperation(uint16_t inst) {
    uint8_t func3 = (inst >> 3) & 0x7;
    switch (func3) {
        case 0b000: return BTOP_BEQ;
        case 0b001: return BTOP_BNE;
        case 0b010: return BTOP_BZ;
        case 0b011: return BTOP_BNZ;
        case 0b100: return BTOP_BLT;
        case 0b101: return BTOP_BGE;
        case 0b110: return BTOP_BLTU;
        case 0b111: return BTOP_BGEU;
        default:    return BTOP_UNKNOWN;
    }
}

std::string Decoder::bTypeOpToString(BTypeOp op) {
    switch (op) {
        case BTOP_BEQ:  return "BEQ";
        case BTOP_BNE:  return "BNE";
        case BTOP_BZ:   return "BZ";
        case BTOP_BNZ:  return "BNZ";
        case BTOP_BLT:  return "BLT";
        case BTOP_BGE:  return "BGE";
        case BTOP_BLTU: return "BLTU";
        case BTOP_BGEU: return "BGEU";
        default:        return "UNKNOWN_B";
    }
}

int16_t Decoder::decodeBTypeOffset(uint16_t inst) {
    uint8_t imm_4_1 = (inst >> 12) & 0xF; // bits 15:12
    int16_t offset = (imm_4_1 << 1); // imm[4:1] + imm[0]=0

    // Sign extend from 5 bits to 16 bits
    if (offset & 0x10) {
        offset |= 0xFFE0;
    }
    return offset;
}

STypeOp Decoder::decodeSTypeOperation(uint16_t inst) {
    uint8_t func3 = (inst >> 3) & 0x7;
    switch (func3) {
        case 0b000: return STOP_SB;
        case 0b001: return STOP_SW;
        default:    return STOP_UNKNOWN;
    }
}

int16_t Decoder::decodeSTypeOffset(uint16_t inst) {
    uint8_t imm4 = (inst >> 12) & 0xF;
    return signExtend4(imm4);
}

std::string Decoder::sTypeOpToString(STypeOp op) {
    switch (op) {
        case STOP_SB: return "SB";
        case STOP_SW: return "SW";
        default:      return "UNKNOWN_S";
    }
}

LTypeOp Decoder::decodeLTypeOperation(uint16_t inst) {
    uint8_t func3 = (inst >> 3) & 0x7;
    switch (func3) {
        case 0b000: return LTOP_LB;
        case 0b001: return LTOP_LW;
        case 0b100: return LTOP_LBU;
        default:    return LTOP_UNKNOWN;
    }
}

int16_t Decoder::decodeLTypeOffset(uint16_t inst) {
    uint8_t imm4 = (inst >> 12) & 0xF;
    return signExtend4(imm4);
}

std::string Decoder::lTypeOpToString(LTypeOp op) {
    switch (op) {
        case LTOP_LB:  return "LB";
        case LTOP_LW:  return "LW";
        case LTOP_LBU: return "LBU";
        default:       return "UNKNOWN_L";
    }
}

JTypeOp Decoder::decodeJTypeOperation(uint16_t inst) {
    bool linkFlag = (inst >> 15) & 0x1;
    return linkFlag ? JTOP_JAL : JTOP_J;
}

int16_t Decoder::decodeJTypeOffset(uint16_t inst) {
    uint16_t imm_high = (inst >> 9) & 0x3F;  // 6 bits imm[9:4]
    uint16_t imm_low  = (inst >> 3) & 0x7;   // 3 bits imm[3:1]
    uint16_t imm = (imm_high << 4) | (imm_low << 1);

    // Sign-extend 10-bit immediate
    if (imm & (1 << 9))
        imm |= 0xFC00;
    return static_cast<int16_t>(imm);
}

std::string Decoder::jTypeOpToString(JTypeOp op) {
    switch (op) {
        case JTOP_J:   return "J";
        case JTOP_JAL: return "JAL";
        default:       return "UNKNOWN_J";
    }
}
UTypeOp Decoder::decodeUTypeOperation(uint16_t inst) {
    bool flag = (inst >> 15) & 0x1;
    return flag ? UTOP_AUIPC : UTOP_LUI;
}

// Add this sign extension function for 9-bit immediates
int16_t Decoder::signExtend9(uint16_t imm9) {
    if (imm9 & 0x100) // if bit 8 (sign bit) is set
        return (int16_t)(imm9 | 0xFE00);
    else
        return (int16_t)(imm9 & 0x1FF);
}

uint16_t Decoder::decodeUTypeImmediate(uint16_t inst) {
    // Extract immediate bits according to U-Type format:
    // [15] flag, [14:9] imm[15:10], [8:6] rd, [5:3] imm[9:7], [2:0] opcode
    uint16_t imm_high = (inst >> 9) & 0x3F;   // 6 bits [14:9] -> imm[15:10]
    uint16_t imm_low  = (inst >> 3) & 0x7;    // 3 bits [5:3] -> imm[9:7]
    uint16_t raw_imm  = (imm_high << 3) | imm_low;  // 9-bit immediate

    // Sign extend the 9-bit immediate, then shift left by 7
    int16_t signed_imm = signExtend9(raw_imm);
    return (uint16_t)(signed_imm << 7);
}

std::string Decoder::uTypeOpToString(UTypeOp op) {
    switch (op) {
        case UTOP_LUI:   return "LUI";
        case UTOP_AUIPC: return "AUIPC";
        default:         return "UNKNOWN_U";
    }
}
        SysTypeOp Decoder::decodeSysTypeOperation(uint16_t inst) {
    uint8_t func3 = (inst >> 3) & 0x7;
    return (func3 == 0b000) ? SYSOP_ECALL : SYSOP_UNKNOWN;
}

uint16_t Decoder::decodeSysCallNumber(uint16_t inst) {
    return (inst >> 6) & 0x3FF; // bits [15:6]
}

std::string Decoder::sysTypeOpToString(SysTypeOp op) {
    switch (op) {
        case SYSOP_ECALL: return "ECALL";
        default:          return "UNKNOWN_SYS";
    }
}

// Utility functions
int16_t Decoder::signExtend4(uint8_t imm4) {
    return (imm4 & 0x8) ? (imm4 | 0xFFF0) : imm4;
}

int16_t Decoder::signExtend7(uint8_t imm7) {
    return (int16_t)((int8_t)(imm7 << 1)) >> 1;
}

int16_t Decoder::signExtend10(uint16_t imm10) {
    if (imm10 & 0x200) // if bit 9 (sign bit) is set
        return (int16_t)(imm10 | 0xFC00);
    else
        return (int16_t)(imm10 & 0x3FF);
}

// MAIN DECODE FUNCTION - CORRECTED FOR ZX16
DecodedInstruction Decoder::decode(uint16_t inst) {
    DecodedInstruction d;
    d.raw = inst;
    d.format = getInstructionFormat(inst);

    switch (d.format) {
        case FORMAT_R: {
            // ZX16 R-type format: [15:12] funct4, [11:9] rs2, [8:6] rd/rs1, [5:3] func3, [2:0] opcode
            d.rd  = (inst >> 6) & 0x7;  // rd/rs1 is both destination and first source
            d.rs1 = d.rd;               // Same as rd for two-operand semantics
            d.rs2 = (inst >> 9) & 0x7;  // Second source register

            d.r_op = decodeRTypeOperation(inst);
            d.mnemonic = rTypeOpToString(d.r_op);

            // Debug output to check register extraction
            // printf("R-type: inst=0x%04x, rd=%d, rs1=%d, rs2=%d, op=%s\n",
            //        inst, d.rd, d.rs1, d.rs2, d.mnemonic.c_str());
            break;
        }

        case FORMAT_I: {
            // ZX16 I-type format: [15:9] imm7, [8:6] rd, [5:3] func3, [2:0] opcode
            d.rd = (inst >> 6) & 0x7;
            d.rs1 = d.rd;  // Two-operand: rd is both destination and first source
            d.i_op = decodeITypeOperation(inst);
            d.mnemonic = iTypeOpToString(d.i_op);

            if (d.i_op == ITOP_SLLI || d.i_op == ITOP_SRLI || d.i_op == ITOP_SRAI) {
                d.imm = (inst >> 9) & 0xF; // 4-bit shift amount
                d.uimm = d.imm;
            } else {
                uint8_t imm7_raw = (inst >> 9) & 0x7F;
                d.imm = signExtend7(imm7_raw);
                d.uimm = d.imm;
            }
            break;
        }

        case FORMAT_B: {
            // ZX16 B-type format: [15:12] imm[4:1], [11:9] rs2, [8:6] rs1, [5:3] func3, [2:0] opcode
            d.rs1 = (inst >> 6) & 0x7;
            d.rs2 = (inst >> 9) & 0x7;
            d.imm = decodeBTypeOffset(inst);
            d.b_op = decodeBTypeOperation(inst);
            d.mnemonic = bTypeOpToString(d.b_op);
            break;
        }

        case FORMAT_S: {
            // ZX16 S-type format: [15:12] imm[3:0], [11:9] rs2, [8:6] rs1, [5:3] func3, [2:0] opcode
            d.rs1 = (inst >> 6) & 0x7;  // base address register
            d.rs2 = (inst >> 9) & 0x7;  // data to store
            d.imm = decodeSTypeOffset(inst);
            d.s_op = decodeSTypeOperation(inst);
            d.mnemonic = sTypeOpToString(d.s_op);
            break;

        }

        case FORMAT_L: {
            // ZX16 L-type format: [15:12] imm[3:0], [11:9] rs2, [8:6] rd, [5:3] func3, [2:0] opcode
            d.rd  = (inst >> 6) & 0x7;   // destination register
            d.rs2 = (inst >> 9) & 0x7;   // base address register
            d.imm = decodeLTypeOffset(inst);
            d.l_op = decodeLTypeOperation(inst);
            d.mnemonic = lTypeOpToString(d.l_op);
            break;
        }

        case FORMAT_J: {
            // ZX16 J-type format: [15] link, [14:9] imm[9:4], [8:6] rd, [5:3] imm[3:1], [2:0] opcode
            d.rd = (inst >> 6) & 0x7;
            d.imm = decodeJTypeOffset(inst);
            d.j_op = decodeJTypeOperation(inst);
            d.mnemonic = jTypeOpToString(d.j_op);
            break;
        }

        case FORMAT_U: {
            // ZX16 U-type format: [15] flag, [14:9] imm[15:10], [8:6] rd, [5:3] imm[9:7], [2:0] opcode
            d.rd = (inst >> 6) & 0x7;

            // Extract immediate bits correctly
            uint16_t imm_high = (inst >> 9) & 0x3F;   // 6 bits [14:9] -> imm[15:10]
            uint16_t imm_low  = (inst >> 3) & 0x7;    // 3 bits [5:3] -> imm[9:7]
            uint16_t raw_imm  = (imm_high << 3) | imm_low;  // 9-bit immediate

            d.u_op = decodeUTypeOperation(inst);
            d.mnemonic = uTypeOpToString(d.u_op);

            // Upper immediate: place 9-bit value in bits [15:7]
            d.imm = raw_imm << 7;
            d.uimm = d.imm;
            d.raw_imm = raw_imm;

            break;
        }
        case FORMAT_SYS: {
            // ZX16 SYS-type format: [15:6] syscall_num, [5:3] func3, [2:0] opcode
            d.syscall_num = decodeSysCallNumber(inst);
            d.sys_op = decodeSysTypeOperation(inst);
            d.mnemonic = sysTypeOpToString(d.sys_op);
            d.imm = d.syscall_num;
            break;
        }

        default:
            d.mnemonic = "UNKNOWN";
            break;
    }

    return d;
}