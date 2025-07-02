#include "Decoder.h"
#include"utils.h"
#include<iostream>
using namespace std;
#include<iomanip>
// The input of this class should be a 16 bit
// inbstructiuon word fetched from memory and it outputs
// a decoded instruction struct containing
// Instruction format (R, I, B, etc.)
//
// Decoded fields (rd, rs1, rs2, immediate, syscall_num, etc.)
//
// Mnemonic (for debugging/logging)
//
// Possibly an enumerated opcode or operation code (to pass to the ALU or execution unit)

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
    uint8_t funct4 = (inst >> 12) & 0xF;
    uint8_t func3  = (inst >> 3)  & 0x7;
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

DecodedInstruction Decoder::decode(uint16_t inst) {
    DecodedInstruction d;
    d.raw = inst;
    d.format = getInstructionFormat(inst);

    switch (d.format) {
        case FORMAT_R: {
            d.rd   = (inst >> 6) & 0x7;
            d.rs1  = d.rd; // shared in R-Type
            d.rs2  = (inst >> 9) & 0x7;
            auto op = decodeRTypeOperation(inst);
            d.mnemonic = rTypeOpToString(op);
            break;
        }
        case FORMAT_I: {
            d.rd = (inst >> 6) & 0x7;
            d.rs1 = (inst >> 3) & 0x7;

            ITypeOp op = decodeITypeOperation(inst);
            d.mnemonic = iTypeOpToString(op);

            if (op == ITOP_SLLI || op == ITOP_SRLI || op == ITOP_SRAI) {
                // Shift immediates are 4-bit unsigned: [0, 15]
                d.imm = (inst >> 9) & 0xF;
                if (d.imm > 15) {
                    d.mnemonic = "INVALID_SHIFT_IMM";
                    std::cerr << "Error: Shift amount " << (int)d.imm
                              << " out of range (0-15) for shift instruction.\n";
                }
            } else {
                // Signed 7-bit immediate: [-64, +63]
                uint8_t imm7_raw = (inst >> 9) & 0x7F;
                int16_t imm = signExtend7(imm7_raw);

                if (imm < -64 || imm > 63) {
                    d.mnemonic = "INVALID_IMM";
                    std::cerr << "Error: Immediate " << imm
                              << " out of 7-bit signed range [-64, 63] for I-type instruction.\n";
                }

                d.imm = imm;
            }
            break;
        }

        case FORMAT_B: {
            d.rs1 = (inst >> 6) & 0x7;
            d.rs2 = (inst >> 9) & 0x7; // ignored for BZ/BNZ but decoded anyway
            d.imm = decodeBTypeOffset(inst);

            BTypeOp op = decodeBTypeOperation(inst);
            d.mnemonic = bTypeOpToString(op);
            break;
        }
        case FORMAT_S: {
            d.rs2 = (inst >> 9) & 0x7;  // data to store
            d.rs1 = (inst >> 6) & 0x7;  // base address
            d.imm = decodeSTypeOffset(inst);

            STypeOp op = decodeSTypeOperation(inst);
            d.mnemonic = sTypeOpToString(op);
            break;
        }
        case FORMAT_L: {
            d.rs2 = (inst >> 9) & 0x7;   // base register
            d.rd  = (inst >> 6) & 0x7;   // destination register
            d.imm = decodeLTypeOffset(inst);

            LTypeOp op = decodeLTypeOperation(inst);
            d.mnemonic = lTypeOpToString(op);
            break;
        }
        case FORMAT_J: {
            d.rd = (inst >> 6) & 0x7; // link register for JAL
            d.imm = decodeJTypeOffset(inst);

            JTypeOp op = decodeJTypeOperation(inst);
            d.mnemonic = jTypeOpToString(op);
            break;
        }
        case FORMAT_U: {
            bool flag = (inst >> 15) & 0x1;
            uint16_t imm_high = (inst >> 9) & 0x3F;   // 6 bits
            uint16_t imm_low  = (inst >> 3) & 0x7;    // 3 bits
            uint16_t raw_imm  = (imm_high << 3) | imm_low;

            d.rd = (inst >> 6) & 0x7;
            d.mnemonic = flag ? "AUIPC" : "LUI";
            d.imm = raw_imm << 7;  // shifted full immediate
            d.uimm = d.imm;

            // Store raw immediate for printing
            d.raw_imm = raw_imm;

            break;
        }


        case FORMAT_SYS: {
            d.syscall_num = decodeSysCallNumber(inst);

            SysTypeOp op = decodeSysTypeOperation(inst);
            d.mnemonic = sysTypeOpToString(op);
            break;
        }

        default:
            d.mnemonic = "UNKNOWN";
    }

    return d;
}
ITypeOp Decoder:: decodeITypeOperation(uint16_t inst) {
    uint8_t imm7 = (inst >> 9) & 0x7F;         // imm[6:0]
    uint8_t imm_shift_type = (imm7 >> 4) & 0x7; // imm[6:4]
    uint8_t shift_amount = imm7 & 0xF;           // imm[3:0]

    uint8_t funct3 = (inst >> 3) & 0x7;

    switch (funct3) {
        case 0b000:

                return ITOP_ADDI;

        case 0b001:
            return ITOP_SLTI;

        case 0b010:
            return ITOP_SLTUI;

        case 0b011:
            if (imm_shift_type == 0b010)  // assuming 010 = SRLI
                return ITOP_SRLI;
            else if (imm_shift_type == 0b100) // assuming 011 = SRAI
                return ITOP_SRAI;
            else
            return ITOP_SLLI;


        case 0b100:
            return ITOP_ORI;

        case 0b101:
            return ITOP_ANDI;

        case 0b110:
            return ITOP_XORI;

        case 0b111:  // if you have LI as a distinct funct3 or encoding
            return ITOP_LI;

        default:
            return ITOP_UNKNOWN;
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
    // Extract imm[4:1]
    uint8_t imm_4_1 = (inst >> 12) & 0xF; // bits 15:12
    // imm[0] = 0 (word aligned, so multiply offset by 2)
    // imm is 5-bit signed (bit 4 is sign bit)

    // Combine and sign extend:
    int16_t offset = (imm_4_1 << 1); // imm[4:1] + imm[0]=0

    // Sign bit is bit 5 (bit 4 of imm_4_1)
    if (offset & 0x10) {
        // Negative number: sign extend 5 bits to 16 bits
        offset |= 0xFFE0; // set upper bits to 1
    }

    return offset; // in bytes (word aligned)
}
STypeOp Decoder::decodeSTypeOperation(uint16_t inst) {
    uint8_t func3 = (inst >> 3) & 0x7;

    switch (func3) {
        case 0b000: return STOP_SB;
        case 0b001: return STOP_SW;
        default:    return STOP_UNKNOWN;
    }
}
int16_t Decoder::signExtend4(uint8_t imm4) {
    // imm4 is 4-bit signed (bit 3 is sign bit)
    return (imm4 & 0x8) ? (imm4 | 0xFFF0) : imm4;
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
// Assuming signExtend4 is already defined
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

    if (linkFlag)
        return JTOP_JAL;
    else
        return JTOP_J;
}
int16_t Decoder::decodeJTypeOffset(uint16_t inst) {
    uint16_t imm_high = (inst >> 9) & 0x3F;  // 6 bits imm[9:4]
    uint16_t imm_low  = (inst >> 3) & 0x7;   // 3 bits imm[3:1]

    uint16_t imm = (imm_high << 4) | (imm_low << 1);

    // Sign-extend 10-bit immediate (bit 9 is sign bit)
    if (imm & (1 << 9))
        imm |= 0xFC00; // set upper bits to 1
    return static_cast<int16_t>(imm);
}
std::string Decoder::jTypeOpToString(JTypeOp op) {
    switch (op) {
        case JTOP_J:   return "J";
        case JTOP_JAL: return "JAL";
        default:      return "UNKNOWN_J";
    }
}
UTypeOp Decoder::decodeUTypeOperation(uint16_t inst) {
    bool flag = (inst >> 15) & 0x1;  // MSB indicates AUIPC if set, else LUI
    if (flag) return UTOP_AUIPC;
    else      return UTOP_LUI;
}


uint16_t Decoder::decodeUTypeImmediate(uint16_t inst) {
    uint16_t imm = (inst >> 7) & 0x1FF;  // Extract 9-bit immediate (bits 15:7)
    return imm << 7;                     // Shift left 7 bits as per spec
}


std::string Decoder::uTypeOpToString(UTypeOp op) {
    switch (op) {
        case UTOP_LUI:   return "LUI";
        case UTOP_AUIPC: return "AUIPC";
        default:        return "UNKNOWN_U";
    }
}
SysTypeOp Decoder::decodeSysTypeOperation(uint16_t inst) {
    uint8_t func3 = (inst >> 3) & 0x7;
    if (func3 == 0b000) {
        return SYSOP_ECALL;
    }
    return SYSOP_UNKNOWN;
}
uint16_t Decoder::decodeSysCallNumber(uint16_t inst) {
    return (inst >> 6) & 0x3FF;  // bits [15:6]
}
std::string Decoder::sysTypeOpToString(SysTypeOp op) {
    switch (op) {
        case SYSOP_ECALL:  return "ECALL";
        default:          return "UNKNOWN_SYS";
    }
}
int16_t Decoder:: signExtend7(uint8_t imm7) {
    if (imm7 & 0x40) // if bit 6 (sign bit) is set
        return (int16_t)(imm7 | 0xFF80); // extend with 1s on top
    else
        return (int16_t)(imm7 & 0x7F);
}
