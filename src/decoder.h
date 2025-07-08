#pragma once
#include <string>
#include <cstdint>
#include "utils.h"

// Instruction formats
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

// I-Type operations
enum ITypeOp {
    ITOP_ADDI = 1, ITOP_SLTI, ITOP_SLTUI,
    ITOP_SLLI, ITOP_SRLI, ITOP_SRAI,
    ITOP_ORI, ITOP_ANDI, ITOP_XORI,
    ITOP_LI,
    ITOP_UNKNOWN = -1
};

// B-Type operations
enum BTypeOp {
    BTOP_BEQ = 0, BTOP_BNE, BTOP_BZ, BTOP_BNZ,
    BTOP_BLT, BTOP_BGE, BTOP_BLTU, BTOP_BGEU,
    BTOP_UNKNOWN = -1
};

// S-Type operations
enum STypeOp {
    STOP_SB = 0, STOP_SW, STOP_UNKNOWN = -1
};

// L-Type operations
enum LTypeOp {
    LTOP_LB = 0, LTOP_LW, LTOP_LBU, LTOP_UNKNOWN = -1
};

// J-Type operations
enum JTypeOp {
    JTOP_J = 0, JTOP_JAL, JTOP_UNKNOWN = -1
};

// U-Type operations
enum UTypeOp {
    UTOP_LUI = 0, UTOP_AUIPC, UTOP_UNKNOWN = -1
};

// System-Type operations
enum SysTypeOp {
    SYSOP_ECALL = 0, SYSOP_UNKNOWN = -1
};

// Decoded instruction structure
struct DecodedInstruction {
    uint16_t raw;
    InstructionFormat format;
    uint8_t rd = 0, rs1 = 0, rs2 = 0;
    int16_t imm = 0;
    uint16_t uimm = 0;
    uint16_t raw_imm = 0;
    mutable uint16_t syscall_num = 0;
    std::string mnemonic;

    RTypeOp r_op = RTOP_UNKNOWN;
    ITypeOp i_op = ITOP_UNKNOWN;
    BTypeOp b_op = BTOP_UNKNOWN;
    STypeOp s_op = STOP_UNKNOWN;
    LTypeOp l_op = LTOP_UNKNOWN;
    JTypeOp j_op = JTOP_UNKNOWN;
    UTypeOp u_op = UTOP_UNKNOWN;
    SysTypeOp sys_op = SYSOP_UNKNOWN;
};

class Decoder {
public:
    DecodedInstruction decode(uint16_t instruction);

private:
    InstructionFormat getInstructionFormat(uint16_t inst);
    RTypeOp decodeRTypeOperation(uint16_t inst);
    ITypeOp decodeITypeOperation(uint16_t inst);
    BTypeOp decodeBTypeOperation(uint16_t inst);
    STypeOp decodeSTypeOperation(uint16_t inst);
    LTypeOp decodeLTypeOperation(uint16_t inst);
    JTypeOp decodeJTypeOperation(uint16_t inst);
    UTypeOp decodeUTypeOperation(uint16_t inst);

    int16_t signExtend9(uint16_t imm9);

    SysTypeOp decodeSysTypeOperation(uint16_t inst);

    std::string rTypeOpToString(RTypeOp op);
    std::string iTypeOpToString(ITypeOp op);
    std::string bTypeOpToString(BTypeOp op);
    std::string sTypeOpToString(STypeOp op);
    std::string lTypeOpToString(LTypeOp op);
    std::string jTypeOpToString(JTypeOp op);
    std::string uTypeOpToString(UTypeOp op);
    std::string sysTypeOpToString(SysTypeOp op);

    int16_t decodeBTypeOffset(uint16_t inst);
    int16_t decodeSTypeOffset(uint16_t inst);
    int16_t decodeLTypeOffset(uint16_t inst);
    int16_t decodeJTypeOffset(uint16_t inst);
    uint16_t decodeUTypeImmediate(uint16_t inst);
    uint16_t decodeSysCallNumber(uint16_t inst);

    // Sign extension helpers
    int16_t signExtend4(uint8_t imm4);
    int16_t signExtend7(uint8_t imm7);
    int16_t signExtend10(uint16_t imm10);
};
