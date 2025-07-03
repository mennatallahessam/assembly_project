#ifndef ECALLS_H
#define ECALLS_H

#include <cstdint>
#include "registers.h"
#include "memory.h"
#include"decoder.h"
// Forward declaration - you'll need to provide this header
class graphics;

class Ecalls {
public:
    // Handle ecall based on the decoded instruction
    void handle(const DecodedInstruction& instr, Registers& regs, Memory& mem, bool& halted, graphics& gfx);

private:
    // Individual service handlers
    void readString(Registers& regs, Memory& mem);
    void readInteger(Registers& regs, Memory& mem);
    void printString(Registers& regs, Memory& mem);
    void printInteger(Registers& regs, Memory& mem);
    void printChar(Registers& regs, Memory& mem);
    void playTone(Registers& regs, Memory& mem, graphics& gfx);
    void setAudioVolume(Registers& regs, Memory& mem, graphics& gfx);
    void stopAudio(Registers& regs, Memory& mem, graphics& gfx);
    void readKeyboard(Registers& regs, Memory& mem, graphics& gfx);
    void registersDump(Registers& regs);
    void memoryDump(Registers& regs, Memory& mem);
    void programExit(bool& halted);
};

#endif // ECALLS_H
