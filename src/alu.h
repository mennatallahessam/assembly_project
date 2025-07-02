#pragma once
#include "DecodedInstruction.h"
#include "Registers.h"
#include "Memory.h"

class ALU {
public:
    void execute(const DecodedInstruction& instr, Registers& regs, Memory& mem, uint32_t& pc, bool& halted);

private:
    void executeRType(const DecodedInstruction& instr, Registers& regs);
    void executeIType(const DecodedInstruction& instr, Registers& regs, uint32_t& pc);
    void executeLoad(const DecodedInstruction& instr, Registers& regs, Memory& mem);
    void executeStore(const DecodedInstruction& instr, Registers& regs, Memory& mem);
    void executeBranch(const DecodedInstruction& instr, Registers& regs, uint32_t& pc);
    void executeJAL(const DecodedInstruction& instr, Registers& regs, uint32_t& pc);
    void executeJALR(const DecodedInstruction& instr, Registers& regs, uint32_t& pc);
    void executeLUI(const DecodedInstruction& instr, Registers& regs);
    void executeAUIPC(const DecodedInstruction& instr, Registers& regs, uint32_t pc);
    void executeECALL(Registers& regs, Memory& mem, bool& halted);
};
