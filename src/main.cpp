#include <iostream>
#include <fstream>
#include <vector>
#include <iomanip>
#include <limits>

#include "Decoder.h"
#include "Registers.h"
#include "Memory.h"
#include "graphics.h"
#include "Ecalls.h"
#include "alu.h"

using namespace std;

std::vector<uint16_t> readBinaryFile(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    std::vector<uint16_t> instructions;

    if (!file) {
        std::cerr << "Error: Cannot open file " << filename << std::endl;
        return instructions;
    }

    while (true) {
        uint16_t word = 0;
        file.read(reinterpret_cast<char*>(&word), sizeof(word));
        if (file.gcount() < sizeof(word)) break;
        instructions.push_back(word);
    }

    return instructions;
}

// Helper function to format instruction with registers
std::string formatInstruction(const DecodedInstruction& d) {
    std::string result = d.mnemonic;

    // Check for NOP (ADD x0, x0, x0)
    if (d.format == FORMAT_R && d.mnemonic == "ADD" && d.rd == 0 && d.rs1 == 0 && d.rs2 == 0) {
        return "NOP";
    }

    switch (d.format) {
        case FORMAT_R:
            if (d.mnemonic == "MV") {
                result += " x" + std::to_string(d.rd) + ", x" + std::to_string(d.rs2);
            }
            else if (d.mnemonic == "JR") {
                result += " x" + std::to_string(d.rd);
            }
            else if (d.mnemonic == "JALR") {
                result += " x" + std::to_string(d.rd) + ", x" + std::to_string(d.rs2);
            }
            else {
                result += " x" + std::to_string(d.rd) + ", x" + std::to_string(d.rs2);
            }
            break;

        case FORMAT_I:
            result += " x" + std::to_string(d.rd) + ", " + std::to_string(d.imm);
            break;

        case FORMAT_S:
            result += " x" + std::to_string(d.rs2) + ", " + std::to_string(d.imm) + "(x" + std::to_string(d.rs1) + ")";
            break;

        case FORMAT_L:
            result += " x" + std::to_string(d.rd) + ", " + std::to_string(d.imm) + "(x" + std::to_string(d.rs2) + ")";
            break;

        case FORMAT_B:
            result += " x" + std::to_string(d.rs1) + ", x" + std::to_string(d.rs2) + ", " + std::to_string(d.imm);
            break;

        case FORMAT_J:
            if (d.rd != 0) {
                result += " x" + std::to_string(d.rd) + ", " + std::to_string(d.imm);
            } else {
                result += " " + std::to_string(d.imm);
            }
            break;

        case FORMAT_U:
            result += " x" + std::to_string(d.rd) + ", " + std::to_string(d.imm);
            break;

        case FORMAT_SYS:
            if (d.sys_op == SYSOP_ECALL) {
                result += " " + std::to_string(d.imm);
            }
            break;
    }

    return result;
}

// Helper function to wait for Enter key press
void waitForEnter() {
    std::cout << "Press Enter to continue..." << std::endl;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

int main() {
    std::string programPath = "C:/Users/ASUS/Desktop/z16/assembler/comprehensivetest.bin";
    std::vector<uint16_t> instructions = readBinaryFile(programPath);

    if (instructions.empty()) {
        std::cerr << "No instructions loaded, exiting." << std::endl;
        return 1;
    }

    Decoder decoder;
    Registers regs;
    Memory mem;
    graphics gfx;
    Ecalls ecalls;
    ALU alu;  // Create ALU instance

    // Load instructions into memory starting at 0x0000
    for (size_t i = 0; i < instructions.size(); ++i) {
        mem.store16(i * 2, instructions[i]);
    }

    uint16_t pc = 0;
    bool halted = false;

    std::cout << "Starting program execution. Press Enter after each instruction to continue.\n" << std::endl;

    while (!halted) {
        uint16_t inst = mem.load16(pc);
        DecodedInstruction d = decoder.decode(inst);

        // Check if this is a NOP and skip display if desired
        bool is_nop = (d.format == FORMAT_R && d.mnemonic == "ADD" && d.rd == 0 && d.rs1 == 0 && d.rs2 == 0);

        // Display instruction (skip NOPs)
        if (!is_nop) {
            std::string formatted = formatInstruction(d);
            std::cout << std::hex << std::setw(4) << std::setfill('0') << pc << ": " << formatted << std::endl;
        }

        // Handle ECALL specially since it needs syscall_num set
        if (d.format == FORMAT_SYS && d.sys_op == SYSOP_ECALL) {
            // For ZX16, the syscall number comes from the instruction immediate field
            // This is different from standard RISC-V but matches your assembly output
            DecodedInstruction ecall_instr = d;
            ecall_instr.syscall_num = d.imm;  // Use immediate field as syscall number

            // Debug output to see what we're executing
          /*  std::cout << "[ECALL DEBUG] Executing syscall " << d.imm
                      << " with a0=" << regs[6] << ", a1=" << regs[7] << std::endl;
*/
            ecalls.handle(ecall_instr, regs, mem, halted, gfx);
            pc += 2;
        } else {
            // Store current PC for ALU execution
            uint16_t next_pc = pc + 2;

            // Execute instruction using ALU
            alu.execute(d, regs, mem, next_pc, halted, ecalls, gfx);

            // Update PC
            pc = next_pc;
        }
        regs.setPC(pc);

        // Wait for Enter key press after each instruction (except NOPs)
        if (!is_nop && !halted) {
            waitForEnter();
        }
    }

    std::cout << "\nProgram execution completed." << std::endl;
    return 0;
}