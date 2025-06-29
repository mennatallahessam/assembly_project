#include "ecalls.h"
#include <iostream>
#include <string>

#define MAX_STRING_LENGTH 1024

Ecalls::Ecalls(Memory& mem, Registers& regs)
    : memory(mem), registers(regs) {}

bool Ecalls::tryHandle(const DecodedInstr& instr, uint16_t& pc) {
    if (instr.opcode != 0xF)
        return false;

    uint8_t serviceNum = registers[10];  // a0

    switch (serviceNum) {
        case 1: {
            printInt();
            return true;
        }
        case 3: {
            printString();
            return true;
        }
        case 10: {
            programExit();
            return true;
        }
        default:
            std::cout << "Unknown ecall: " << (int)serviceNum << std::endl;
        return false;
    }
}

void Ecalls::printInt() {
    uint16_t value = registers[11];  // print value in r11
    std::cout << value << std::endl;
}

void Ecalls::printString() {
    uint16_t addr = registers[11];
    std::string output;
    char c;
    size_t count = 0;

    while ((c = memory.readByte(addr++)) != 0 && count < MAX_STRING_LENGTH) {
        output += c;
        count++;
    }

    if (count >= MAX_STRING_LENGTH) {
        std::cout << "Error: String too long or unterminated." << std::endl;
        return;
    }

    std::cout << output << std::endl;
}

void Ecalls::programExit() {
    std::cout << "Program exited." << std::endl;
}
