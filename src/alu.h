#ifndef ALU_H
#define ALU_H

#include "decoder.h"  // For DecodedInstr structure
#include "registers.h"
#include "memory.h"

class ALU {
public:
    void execute(const DecodedInstr& instr, Registers& regs, Memory& mem, uint16_t& pc);
};

#endif // ALU_H