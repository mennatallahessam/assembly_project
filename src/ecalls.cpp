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