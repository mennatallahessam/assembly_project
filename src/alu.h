#pragma once
#include "decoder.h"
#include "registers.h"
#include "memory.h"
#include "ecalls.h"
#include "graphics.h"

class ALU {
public:
    void execute(const DecodedInstruction& instr, Registers& regs, Memory& mem, uint16_t& pc, bool& halted, Ecalls& ecalls, graphics& gfx);
};
