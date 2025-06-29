#ifndef ECALLS_H
#define ECALLS_H

#include "memory.h"
#include "registers.h"
#include "decoder.h"

class Ecalls {
public:
    Ecalls(Memory& mem, Registers& regs);

    // Try to handle ecall, returns true if program halted
    bool tryHandle(const DecodedInstr& instr, uint16_t& pc);

    void printInt();

private:
    Memory& memory;
    Registers& registers;

    // Implement system calls below
    void printString();
    void programExit();

    // Add other system call handlers (playTone, stopAudio, etc.)
};

#endif // ECALLS_H
