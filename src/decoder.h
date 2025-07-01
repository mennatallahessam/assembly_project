#pragma once
#include <string>
#include <cstdint>
#include "utils.h"
// Formats
enum InstructionFormat {
    FORMAT_R, FORMAT_I, FORMAT_B, FORMAT_S,
    FORMAT_L, FORMAT_J, FORMAT_U, FORMAT_SYS,
    FORMAT_UNKNOWN
};


// R-Type operations
enum RTypeOp {
    RTOP_ADD = 1, RTOP_SUB, RTOP_SLT, RTOP_SLTU,
    RTOP_SLL, RTOP_SRL, RTOP_SRA,
    RTOP_OR, RTOP_AND, RTOP_XOR,
    RTOP_MV, RTOP_JR, RTOP_JALR,
    RTOP_UNKNOWN = -1
};
// I- Type operations
enum ITypeOp {
    ITOP_ADDI = 1, ITOP_SLTI, ITOP_SLTUI,
    ITOP_SLLI, ITOP_SRLI, ITOP_SRAI,
    ITOP_ORI, ITOP_ANDI, ITOP_XORI,
    ITOP_LI,
    ITOP_UNKNOWN = -1
};
// B-Type operations
enum BTypeOp {
    BTOP_BEQ = 0,
    BTOP_BNE,
    BTOP_BZ,
    BTOP_BNZ,
    BTOP_BLT,
    BTOP_BGE,
    BTOP_BLTU,
    BTOP_BGEU,
    BTOP_UNKNOWN = -1
};
//S-Type operations
enum STypeOp {
    STOP_SB = 0,
    STOP_SW,
    STOP_UNKNOWN = -1
};
//L - type operations
enum LTypeOp {
    LTOP_LB = 0,
    LTOP_LW,
    LTOP_LBU,
    LTOP_UNKNOWN = -1
};
//J-type
enum JTypeOp {
    JTOP_J = 0,
    JTOP_JAL,
    JTOP_UNKNOWN = -1
};
//U-type operations
enum UTypeOp {
    UTOP_LUI = 0,
    UTOP_AUIPC,
    UTOP_UNKNOWN = -1
};
//SYS - operations
enum SysTypeOp {
    SYSOP_ECALL = 0,
    SYSOP_UNKNOWN = -1
};

// Decoded instruction container
struct DecodedInstruction {
    uint16_t raw;
    InstructionFormat format;
    uint8_t rd, rs1, rs2;
    int16_t imm;            // for immediate values / offsets
    std::string mnemonic;
    uint16_t syscall_num;   // for SYS-Type instructions
};


class Decoder {
public:
    DecodedInstruction decode(uint16_t instruction);

    BTypeOp decodeBTypeOperation(uint16_t inst);

    std::string bTypeOpToString(BTypeOp op);

    int16_t decodeBTypeOffset(uint16_t inst);

    STypeOp decodeSTypeOperation(uint16_t inst);

    int16_t signExtend4(uint8_t imm4);

    int16_t decodeSTypeOffset(uint16_t inst);

    std::string sTypeOpToString(STypeOp op);

    LTypeOp decodeLTypeOperation(uint16_t inst);

    int16_t decodeLTypeOffset(uint16_t inst);

    std::string lTypeOpToString(LTypeOp op);

    JTypeOp decodeJTypeOperation(uint16_t inst);

    int16_t decodeJTypeOffset(uint16_t inst);

    std::string jTypeOpToString(JTypeOp op);

    UTypeOp decodeUTypeOperation(uint16_t inst);

    uint16_t decodeUTypeImmediate(uint16_t inst);

    std::string uTypeOpToString(UTypeOp op);

    SysTypeOp decodeSysTypeOperation(uint16_t inst);

    uint16_t decodeSysCallNumber(uint16_t inst);

    std::string sysTypeOpToString(SysTypeOp op);

    int16_t signExtend7(uint8_t imm7);

private:
    InstructionFormat getInstructionFormat(uint16_t inst);
    RTypeOp decodeRTypeOperation(uint16_t inst);
    std::string rTypeOpToString(RTypeOp op);
    ITypeOp decodeITypeOperation(uint16_t inst);
    std::string iTypeOpToString(ITypeOp op);

};
