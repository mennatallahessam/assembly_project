// Modified main.cpp with graphics integration
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
#include "DataLoader.h"
#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>

using namespace std;

// Keep your existing functions
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

void waitForEnter() {
    std::cout << "Press Enter to continue..." << std::endl;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

// Graphics test function
void setupGraphicsTest(Memory& mem, graphics& gfx) {
    std::cout << "\n=== Setting up Graphics Test ===" << std::endl;

    // Set up test color palette
    gfx.setTestPalette(mem);

    // Create a simple test pattern in tile map
    try {
        // Fill tile map with alternating tile patterns
        for (int i = 0; i < 300; i++) {
            uint8_t tileIndex = (i % 4); // Use tiles 0-3
            mem.store8(0xF000 + i, tileIndex);
        }

        // Create simple test tiles
        // Tile 0: Solid color (color index 1 - blue)
        for (int i = 0; i < 128; i++) {
            mem.store8(0xF200 + i, 0x11); // Both pixels color 1
        }

        // Tile 1: Checkered pattern (colors 2 and 3)
        for (int i = 0; i < 128; i++) {
            int pixelPair = i;
            int row = (pixelPair * 2) / 16;
            int col = (pixelPair * 2) % 16;

            bool checker = ((row / 2) + (col / 2)) % 2;
            uint8_t colorPair = checker ? 0x23 : 0x32; // Alternate colors 2 and 3
            mem.store8(0xF200 + 128 + i, colorPair);
        }

        // Tile 2: Gradient pattern
        for (int i = 0; i < 128; i++) {
            uint8_t color = (i * 15) / 127; // Gradient from 0 to 15
            mem.store8(0xF200 + 256 + i, (color << 4) | color);
        }

        // Tile 3: Border pattern
        for (int i = 0; i < 128; i++) {
            int pixelPair = i;
            int row = (pixelPair * 2) / 16;
            int col = (pixelPair * 2) % 16;

            bool border = (row == 0 || row == 15 || col == 0 || col == 15);
            uint8_t colorPair = border ? 0x77 : 0x00; // White border, black center
            mem.store8(0xF200 + 384 + i, colorPair);
        }

        std::cout << "Graphics test pattern created" << std::endl;

    } catch (const AddressOutOfBoundsException& e) {
        std::cerr << "Error setting up graphics test: " << e.what() << std::endl;
    }
}

int main() {
    Decoder decoder;
    Registers regs;
    Memory mem;
    graphics gfx;
    Ecalls ecalls;
    ALU alu;
    DataSection dataSection;

    // Initialize graphics system
    bool graphicsReady = gfx.initializeGraphics();
    if (graphicsReady) {
        std::cout << "Graphics system initialized successfully!" << std::endl;
        setupGraphicsTest(mem, gfx);

        // Initial graphics update
        gfx.updateGraphics(mem);
        gfx.renderFrame();

        std::cout << "\nGraphics Controls:" << std::endl;
        std::cout << "  P - Display color palette info" << std::endl;
        std::cout << "  T - Show color palette test" << std::endl;
        std::cout << "  Close graphics window to exit graphics mode" << std::endl;
    }

    // Load your program
    std::string programPath = "C:/Users/ASUS/Desktop/z16-fork/assembler/lasttest.bin";
    std::vector<uint16_t> instructions = readBinaryFile(programPath);

    if (instructions.empty()) {
        std::cerr << "No instructions loaded, exiting." << std::endl;
        return 1;
    }

    // Load instructions into memory
    for (size_t i = 0; i < instructions.size(); ++i) {
        mem.store16(i * 2, instructions[i]);
    }

    uint16_t pc = 0;
    bool halted = false;

    std::cout << "\nStarting program execution." << std::endl;
    if (graphicsReady) {
        std::cout << "Graphics window is open - you can interact with it while the program runs." << std::endl;
    }
    std::cout << "Press Enter after each instruction to continue.\n" << std::endl;

    while (!halted) {
        // Handle graphics events (non-blocking)
        if (graphicsReady && gfx.isGraphicsWindowOpen()) {
            gfx.handleGraphicsEvents();
        }

        uint16_t inst = mem.load16(pc);
        DecodedInstruction d = decoder.decode(inst);

        bool is_nop = (d.format == FORMAT_R && d.mnemonic == "ADD" && d.rd == 0 && d.rs1 == 0 && d.rs2 == 0);

        if (!is_nop) {
            std::string formatted = formatInstruction(d);
            std::cout << std::hex << std::setw(4) << std::setfill('0') << pc << ": " << formatted << std::endl;
        }

        // Execute instruction
        if (d.format == FORMAT_SYS && d.sys_op == SYSOP_ECALL) {
            DecodedInstruction ecall_instr = d;
            ecall_instr.syscall_num = d.imm;
            ecalls.handle(ecall_instr, regs, mem, halted, gfx);
            pc += 2;
        } else {
            uint16_t next_pc = pc + 2;
            alu.execute(d, regs, mem, next_pc, halted, ecalls, gfx);
            pc = next_pc;
        }
        regs.setPC(pc);

        // Update graphics after each instruction (if graphics are enabled)
        if (graphicsReady && gfx.isGraphicsWindowOpen()) {
            gfx.updateGraphics(mem);
            gfx.renderFrame();
        }

        // Wait for input (except for NOPs)
        if (!is_nop && !halted) {
            waitForEnter();
        }
    }

    std::cout << "\nProgram execution completed." << std::endl;

    // Keep graphics window open for a bit after program ends
    if (graphicsReady && gfx.isGraphicsWindowOpen()) {
        std::cout << "Graphics window is still open. Close it to exit." << std::endl;
        while (gfx.isGraphicsWindowOpen()) {
            gfx.handleGraphicsEvents();
            gfx.renderFrame();
            sf::sleep(sf::milliseconds(16)); // ~60 FPS
        }
    }

    return 0;
}