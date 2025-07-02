// #include "ecalls.h"
// #include <iostream>
// #include <string>
// #include <iomanip>
//
// #define MAX_STRING_LENGTH 1024
//
// Ecalls::Ecalls(Memory& mem, Registers& regs)
//     : memory(mem), registers(regs) {}
//
// bool Ecalls::tryHandle(const DecodedInstr& instr, uint16_t& pc) {
//     if (instr.opcode != 0xF)
//         return false;
//
//     uint8_t serviceNum = registers[10];  // r10 = syscall number (a0)
//
//     switch (serviceNum) {
//         case 1:  // print integer
//             printInt();
//             return false;
//
//         case 3:  // print string
//             printString();
//             return false;
//
//         case 6:  // print character
//             printChar();
//             return false;
//
//         case 10: // exit
//             programExit();
//             return true;
//
//         default:
//             std::cout  << (int)serviceNum << std::endl;
//             return false;
//     }
// }
//
// void Ecalls::printInt() {
//     int16_t value = static_cast<int16_t>(registers[11]);  // r11 = integer value
//     std::cout << value;
// }
//
// void Ecalls::printString() {
//     uint16_t addr = registers[11];  // r11 = address of string
//     std::string output;
//     char c;
//     size_t count = 0;
//
//     while ((c = memory.readByte(addr++)) != 0 && count < MAX_STRING_LENGTH) {
//         output += c;
//         count++;
//     }
//
//     if (count >= MAX_STRING_LENGTH) {
//         std::cout << "Error: String too long or unterminated." << std::endl;
//         return;
//     }
//
//     std::cout << output;
// }
//
// void Ecalls::printChar() {
//     char c = static_cast<char>(registers[11] & 0xFF);  // r11 = character value
//
//     if (c >= 32 && c <= 126) {
//         // Printable ASCII character
//         std::cout << c;
//     } else {
//         // Non-printable character - show as escape sequence
//         switch (c) {
//             case 0: std::cout << "\\0"; break;
//             case 7: std::cout << "\\a"; break;
//             case 8: std::cout << "\\b"; break;
//             case 9: std::cout << "\\t"; break;
//             case 10: std::cout << "\\n"; break;
//             case 13: std::cout << "\\r"; break;
//             default:
//                 std::cout << "\\x" << std::hex << std::setw(2)
//                          << std::setfill('0') << (int)(c & 0xFF) << std::dec;
//         }
//     }
// }
//
// void Ecalls::programExit() {
//     std::cout << std::endl << "Program exited." << std::endl;
// }
#include "ecalls.h"
#include <iostream>
#include <string>

void Ecalls::handle(Registers& regs, Memory& mem, bool& halted, Graphics& gfx) {
    uint16_t service = regs[0]; // a0 or service reg, per ZX16 calling convention
    switch (service) {
        case 1: { // Read String
            uint16_t addr = regs[0];
            uint16_t maxlen = regs[1];
            std::string input;
            std::getline(std::cin, input);
            if (input.length() > maxlen) input.resize(maxlen);
            for (size_t i = 0; i < input.length(); ++i)
                mem.writeByte(addr + i, input[i]);
            mem.writeByte(addr + input.length(), 0);
            regs[0] = input.length();
            break;
        }
        case 2: { // Read Integer
            int val;
            std::cin >> val;
            regs[0] = val;
            break;
        }
        case 3: { // Print String
            uint16_t addr = regs[0];
            char c;
            while ((c = mem.readByte(addr++)) != '\0')
                std::cout << c;
            break;
        }
        case 4: // Play tone
            // Implement as needed for your environment or stub
            break;
        case 5: // Set audio volume
            // Implement as needed for your environment or stub
            break;
        case 6: // Stop Audio playback
            // Implement as needed for your environment or stub
            break;
        case 7: // Read the keyboard
            // Implement as needed for your environment or stub
            break;
        case 8: // Registers Dump
            regs.dump();
            break;
        case 9: { // Memory Dump
            uint16_t addr = regs[0];
            uint16_t count = regs[1];
            for (uint16_t i = 0; i < count; ++i)
                std::cout << std::hex << int(mem.readByte(addr + i)) << " ";
            std::cout << std::endl;
            break;
        }
        case 10: // Program Exit
            halted = true;
            break;
        default:
            std::cerr << "Unknown ecall service: " << service << std::endl;
            halted = true;
    }
}
