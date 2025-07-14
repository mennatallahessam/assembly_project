#include <iostream>
#include <fstream>
#include <vector>
#include <iomanip>
#include <limits>
#include <chrono>
#include <thread>
#include "Decoder.h"
#include "Registers.h"
#include "Memory.h"
#include "Graphics.h"
#include "Ecalls.h"
#include "alu.h"
#include "DataLoader.h"

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
            if (d.mnemonic == "BZ" || d.mnemonic == "BNZ") {
                result += " x" + std::to_string(d.rs1) + ", " + std::to_string(d.imm);
            } else {
                result += " x" + std::to_string(d.rs1) + ", x" + std::to_string(d.rs2) + ", " + std::to_string(d.imm);
            }
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

void setupGraphicsDemo(Memory& mem) {
    std::cout << "Setting up graphics demo with visible colors..." << std::endl;

    // 1. Set up color palette at 0xFA00 with HIGH CONTRAST colors
    mem.store8(0xFA00 + 0, 0xE0);  // Bright Red: 111 000 00
    mem.store8(0xFA00 + 1, 0x03);  // Bright Blue: 000 000 11
    mem.store8(0xFA00 + 2, 0x1C);  // Bright Green: 000 111 00
    mem.store8(0xFA00 + 3, 0xFC);  // Yellow: 111 111 00
    mem.store8(0xFA00 + 4, 0xFF);  // White: 111 111 11
    mem.store8(0xFA00 + 5, 0x00);  // Black: 000 000 00

    // Initialize remaining palette to distinct colors
    for (int i = 6; i < 16; i++) {
        mem.store8(0xFA00 + i, 0x00);  // Black for unused
    }

    // 2. Define tiles with SOLID colors
    // Tile 0: Pure Red (palette index 0)
    for (int i = 0; i < 128; i++) {
        mem.store8(0xF200 + i, 0x00);  // 0x00 = palette[0] in both nibbles
    }

    // Tile 1: Pure Blue (palette index 1)
    for (int i = 0; i < 128; i++) {
        mem.store8(0xF200 + 128 + i, 0x11);  // 0x11 = palette[1] in both nibbles
    }

    // Tile 2: Pure Green (palette index 2)
    for (int i = 0; i < 128; i++) {
        mem.store8(0xF200 + 256 + i, 0x22);  // 0x22 = palette[2] in both nibbles
    }

    // Tile 3: Pure Yellow (palette index 3)
    for (int i = 0; i < 128; i++) {
        mem.store8(0xF200 + 384 + i, 0x33);  // 0x33 = palette[3] in both nibbles
    }

    // 3. Create a clear pattern in the tile map
    // Clear the entire tile map first
    for (int i = 0; i < 300; i++) {
        mem.store8(0xF000 + i, 255);  // Invalid tile (should render as black/transparent)
    }

    // Create vertical stripes pattern
    for (int row = 0; row < 15; row++) {
        for (int col = 0; col < 20; col++) {
            uint16_t addr = 0xF000 + row * 20 + col;
            uint8_t tile_index;

            if (col < 5) {
                tile_index = 0;  // Red columns (0-4)
            } else if (col < 10) {
                tile_index = 1;  // Blue columns (5-9)
            } else if (col < 15) {
                tile_index = 2;  // Green columns (10-14)
            } else {
                tile_index = 3;  // Yellow columns (15-19)
            }

            mem.store8(addr, tile_index);
        }
    }

    std::cout << "Graphics demo setup complete!" << std::endl;
    std::cout << "Expected: 4 vertical color stripes - Red, Blue, Green, Yellow" << std::endl;
}

int main() {
    Decoder decoder;
    Registers regs;
    Memory mem;
    Graphics gfx(&mem);
    Ecalls ecalls;
    ALU alu;
    DataSection dataSection;

    // Initialize graphics system
    if (!gfx.initialize()) {
        std::cerr << "Failed to initialize graphics system!" << std::endl;
        return 1;
    }

    // Initialize a simple color palette for testing
    std::cout << "Setting up test color palette..." << std::endl;
    mem.store8(0xFA00, 0xE0);  // Red
    mem.store8(0xFA01, 0x03);  // Blue
    mem.store8(0xFA02, 0x1C);  // Green
    mem.store8(0xFA03, 0xFC);  // Yellow
    mem.store8(0xFA04, 0xFF);  // White
    mem.store8(0xFA05, 0xFC);  // Yellow
    for (int i = 6; i < 16; i++) {
        mem.store8(0xFA00 + i, 0x00);  // Black for unused colors
    }

    std::string programPath = "C:/Users/ASUS/Desktop/z16-fork/assembler/video.bin";
    std::vector<uint16_t> instructions = readBinaryFile(programPath);

    if (instructions.empty()) {
        std::cerr << "No instructions loaded, exiting." << std::endl;
        return 1;
    }

    std::cout << "Loaded " << instructions.size() << " instructions from " << programPath << std::endl;

    // Load instructions into memory starting at 0x0000
    for (size_t i = 0; i < instructions.size(); ++i) {
        mem.store16(i * 2, instructions[i]);
    }

    uint16_t pc = 0;
    bool halted = false;
    int instruction_count = 0;
    int loop_detection_count = 0;
    uint16_t last_pc = 0xFFFF;

    std::cout << "\n" << std::string(50, '=') << std::endl;
    std::cout << "         REAL ZX16 INSTRUCTION SIMULATION" << std::endl;
    std::cout << std::string(50, '=') << std::endl;
    std::cout << "\nRunning ZX16 assembly program..." << std::endl;
    std::cout << "Graphics will be created by simulated ZX16 instructions." << std::endl;

    while (!halted && gfx.isWindowOpen()) {
        uint16_t inst = mem.load16(pc);
        DecodedInstruction d = decoder.decode(inst);

        // Check if this is a NOP and skip display if desired
        bool is_nop = (d.format == FORMAT_R && d.mnemonic == "ADD" && d.rd == 0 && d.rs1 == 0 && d.rs2 == 0);

        // Show first 20 instructions for debugging
        if (!is_nop && instruction_count < 20) {
            std::string formatted = formatInstruction(d);
            std::cout << std::hex << std::setw(4) << std::setfill('0') << pc << ": " << formatted << std::endl;
        }

        // Detect infinite loops
        if (pc == last_pc) {
            loop_detection_count++;
            if (loop_detection_count > 1000) {
                std::cout << "\nINFINITE LOOP DETECTED at PC=0x" << std::hex << pc << std::endl;
                std::cout << "Breaking execution to prevent hang..." << std::endl;
                break;
            }
        } else {
            loop_detection_count = 0;
            last_pc = pc;
        }

        // Handle ECALL specially since it needs syscall_num set
        if (d.format == FORMAT_SYS && d.sys_op == SYSOP_ECALL) {
            DecodedInstruction ecall_instr = d;
            ecall_instr.syscall_num = d.imm;

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
        instruction_count++;

        // Mark graphics for update and render
        gfx.markDirty();
        gfx.update();

        // Show progress occasionally
        if (instruction_count % 1000 == 0) {
            std::cout << "Instructions executed: " << instruction_count << std::endl;
        }

        // Safety check to prevent infinite loops
        if (instruction_count > 100000) {
            std::cout << "Stopping after 100000 instructions to prevent infinite loop." << std::endl;
            break;
        }

        // Small delay to prevent the CPU from running too fast (but not too slow)
        std::this_thread::sleep_for(std::chrono::microseconds(100));
    }

    std::cout << "\nProgram execution completed." << std::endl;
    std::cout << "Instructions executed: " << instruction_count << std::endl;

    // Keep graphics window open
    std::cout << "\nGraphics window will remain open. Close window to exit." << std::endl;
    while (gfx.isWindowOpen()) {
        gfx.update();
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }

    return 0;
}