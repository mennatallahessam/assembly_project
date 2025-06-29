// File: src/alu.h
#ifndef ALU_H
#define ALU_H

#include "decoder.h"
#include "registers.h"

class ALU {
public:
    void execute(const DecodedInstr& instr, Registers& regs);
};

#endif
