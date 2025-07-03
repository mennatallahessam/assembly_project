#include "ecalls.h"
#include "Decoder.h"
#include <iostream>
#include <string>

void Ecalls::handle(const DecodedInstruction& instr, Registers& regs, Memory& mem, bool& halted, graphics& gfx) {
    // The service number comes from the instruction's syscall_num field
    uint16_t service = instr.syscall_num;
    
    switch (service) {
        case 1: // Read String
            readString(regs, mem);
            break;
        case 2: // Read Integer
            readInteger(regs, mem);
            break;
        case 3: // Print String
            printString(regs, mem);
            break;
        case 4: // Print Integer
            printInteger(regs, mem);
            break;
        case 5: // Print Character
            printChar(regs, mem);
            break;
        case 6: // Play tone
            playTone(regs, mem, gfx);
            break;
        case 7: // Set audio volume
            setAudioVolume(regs, mem, gfx);
            break;
        case 8: // Stop Audio playback
            stopAudio(regs, mem, gfx);
            break;
        case 9: // Read the keyboard
            readKeyboard(regs, mem, gfx);
            break;
        case 10: // Registers Dump
            registersDump(regs);
            break;
        case 11: // Memory Dump
            memoryDump(regs, mem);
            break;
        case 12: // Program Exit
            programExit(halted);
            break;
        default:
            std::cerr << "Unknown ecall service: " << service << std::endl;
            halted = true;
    }
}

void Ecalls::readString(Registers& regs, Memory& mem) {
    uint16_t addr = regs[1];    // x1 = address
    uint16_t maxlen = regs[2];  // x2 = max length
    std::string input;
    std::getline(std::cin, input);
    if (input.length() > maxlen) input.resize(maxlen);
    for (size_t i = 0; i < input.length(); ++i)
        mem.writeByte(addr + i, input[i]);
    mem.writeByte(addr + input.length(), 0);
    regs[0] = input.length(); // Return length in x0
}

void Ecalls::readInteger(Registers& regs, Memory& mem) {
    int val;
    std::cin >> val;
    regs[0] = static_cast<uint16_t>(val); // Return value in x0
}

void Ecalls::printString(Registers& regs, Memory& mem) {
    uint16_t addr = regs[1]; // x1 = address of string
    char c;
    while ((c = mem.readByte(addr++)) != '\0')
        std::cout << c;
}

void Ecalls::printInteger(Registers& regs, Memory& mem) {
    int16_t value = static_cast<int16_t>(regs[1]); // x1 = integer value
    std::cout << value;
}

void Ecalls::printChar(Registers& regs, Memory& mem) {
    char c = static_cast<char>(regs[1] & 0xFF); // x1 = character value
    std::cout << c;
}

void Ecalls::playTone(Registers& regs, Memory& mem, graphics& gfx) {
    // Stub implementation - you'll need to implement based on your graphics class
    std::cout << "[TONE: freq=" << regs[1] << ", duration=" << regs[2] << "]" << std::endl;
}

void Ecalls::setAudioVolume(Registers& regs, Memory& mem, graphics& gfx) {
    // Stub implementation
    std::cout << "[VOLUME: " << regs[1] << "]" << std::endl;
}

void Ecalls::stopAudio(Registers& regs, Memory& mem, graphics& gfx) {
    // Stub implementation
    std::cout << "[AUDIO STOP]" << std::endl;
}

void Ecalls::readKeyboard(Registers& regs, Memory& mem, graphics& gfx) {
    // Stub implementation
    char c;
    std::cin >> c;
    regs[0] = static_cast<uint16_t>(c);
}

void Ecalls::registersDump(Registers& regs) {
    regs.dump();
}

void Ecalls::memoryDump(Registers& regs, Memory& mem) {
    uint16_t addr = regs[1];  // x1 = address
    uint16_t count = regs[2]; // x2 = count
    for (uint16_t i = 0; i < count; ++i) {
        std::cout << std::hex << (int)mem.readByte(addr + i) << " ";
    }
    std::cout << std::endl;
}

void Ecalls::programExit(bool& halted) {
    std::cout << std::endl << "Program exited." << std::endl;
    halted = true;
}