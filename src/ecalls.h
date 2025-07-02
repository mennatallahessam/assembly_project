// #ifndef ECALLS_H
// #define ECALLS_H
//
// #include "memory.h"
// #include "registers.h"
// #include "decoder.h"
//
// class Ecalls {
// private:
//     Memory& memory;
//     Registers& registers;
//
// public:
//     Ecalls(Memory& mem, Registers& regs);
//     bool tryHandle(const DecodedInstr& instr, uint16_t& pc);
//
// private:
//     void printInt();
//     void printString();
//     void printChar();    // Add this line
//     void programExit();
// };
//
// #endif
#pragma once
#include "registers.h"
#include "memory.h"
#include "graphics.h"

class Ecalls {
public:
    void handle(Registers& regs, Memory& mem, bool& halted, Graphics& gfx);
};
