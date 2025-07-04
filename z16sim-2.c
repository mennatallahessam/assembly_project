/*
 * Z16 Instruction Set Simulator (ISS)
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at:
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * Author: Mohamed Shalan
 *
 * This simulator accepts a Z16 binary machine code file (with a .bin extension) and assumes that
 * the first instruction is located at memory address 0x0000. It decodes each 16-bit instruction into a
 * human-readable string and prints it, then executes the instruction by updating registers, memory,
 * or performing I/O via ecall.
 *
 * Supported ecall services:
 *   - ecall 1: Print an integer (value in register a0).
 *   - ecall 5: Print a NULL-terminated string (address in register a0).
 *   - ecall 3: Terminate the simulation.
 *
 * Usage:
 *   z16sim <machine_code_file_name>
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>

#define MEM_SIZE 65536  // 64KB memory
uint8_t mem[MEM_SIZE];

// Global simulated memory and register file.
unsigned char memory[MEM_SIZE];
uint16_t regs[8];      // 8 registers (16-bit each): x0, x1, x2, x3, x4, x5, x6, x7
uint16_t pc = 0;       // Program counter (16-bit)

// Register ABI names for display (x0 = t0, x1 = ra, x2 = sp, x3 = s0, x4 = s1, x5 = t1, x6 = a0, x7 = a1)
const char *regNames[8] = {"t0", "ra", "sp", "s0", "s1", "t1", "a0", "a1"};

// -----------------------
// Disassembly Function
// -----------------------
//
// Decodes a 16-bit instruction 'inst' (fetched at address 'pc') and writes a human‑readable
// string to 'buf' (of size bufSize). This decoder uses the opcode (bits [2:0]) to distinguish
// among R‑, I‑, B‑, L‑, J‑, U‑, and System instructions.
void disassemble(uint16_t inst, uint16_t pc, char *buf, size_t bufSize) {
    uint8_t opcode = inst & 0x7;
    switch(opcode) {
        case 0x0: { // R-type: [15:12] funct4 | [11:9] rs2 | [8:6] rd/rs1 | [5:3] funct3 | [2:0] opcode
            uint8_t funct4 = (inst >> 12) & 0xF;
            uint8_t rs2     = (inst >> 9) & 0x7;
            uint8_t rd_rs1  = (inst >> 6) & 0x7;
            uint8_t funct3  = (inst >> 3) & 0x7;
               if      (funct4 == 0x0 && funct3 == 0x0)
                   snprintf(buf, bufSize, "add %s, %s", regNames[rd_rs1], regNames[rs2]);
               else if (funct4 == 0x1 && funct3 == 0x0)
                   snprintf(buf, bufSize, "sub %s, %s", regNames[rd_rs1], regNames[rs2]);
               else if (funct4 == 0x2 && funct3 == 0x1)
                   snprintf(buf, bufSize, "slt %s, %s", regNames[rd_rs1], regNames[rs2]);
               else if (funct4 == 0x3 && funct3 == 0x2)
                   snprintf(buf, bufSize, "sltu %s, %s", regNames[rd_rs1], regNames[rs2]);
               else if (funct4 == 0x4 && funct3 == 0x3)
                   snprintf(buf, bufSize, "sll %s, %s", regNames[rd_rs1], regNames[rs2]);
               else if (funct4 == 0x5 && funct3 == 0x3)
                   snprintf(buf, bufSize, "srl %s, %s", regNames[rd_rs1], regNames[rs2]);
               else if (funct4 == 0x6 && funct3 == 0x3)
                   snprintf(buf, bufSize, "sra %s, %s", regNames[rd_rs1], regNames[rs2]);
               else if (funct4 == 0x7 && funct3 == 0x4)
                   snprintf(buf, bufSize, "or %s, %s", regNames[rd_rs1], regNames[rs2]);
               else if (funct4 == 0x8 && funct3 == 0x5)
                   snprintf(buf, bufSize, "and %s, %s", regNames[rd_rs1], regNames[rs2]);
               else if (funct4 == 0x9 && funct3 == 0x6)
                   snprintf(buf, bufSize, "xor %s, %s", regNames[rd_rs1], regNames[rs2]);
               else if (funct4 == 0xA && funct3 == 0x7)
                   snprintf(buf, bufSize, "mv %s, %s", regNames[rd_rs1], regNames[rs2]);
               else if (funct4 == 0xB && funct3 == 0x0)
                   snprintf(buf, bufSize, "jr %s", regNames[rd_rs1]);
               else if (funct4 == 0xC && funct3 == 0x0)
                   snprintf(buf, bufSize, "jalr %s, %s", regNames[rd_rs1], regNames[rs2]);
               else
                   snprintf(buf, bufSize, "unknown R-type");
            break;
        }
        case 0x1: { // I-type
            uint8_t imm7 = (inst >> 9) & 0x7F;
            uint8_t rd_rs1 = (inst >> 6) & 0x7;
            uint8_t funct3 = (inst >> 3) & 0x7;
            int16_t simm = (imm7 & 0x40) ? (imm7 | 0xFF80) : imm7;  // sign-extend

            if      (funct3 == 0x0)
                snprintf(buf, bufSize, "addi %s, %d", regNames[rd_rs1], simm);
            else if (funct3 == 0x1)
                snprintf(buf, bufSize, "slti %s, %d", regNames[rd_rs1], simm);
            else if (funct3 == 0x2)
                snprintf(buf, bufSize, "sltui %s, %u", regNames[rd_rs1], imm7);
            else if (funct3 == 0x3) {
                uint8_t imm_high = (imm7 >> 4) & 0x7;
                if (imm_high == 0x1)
                    snprintf(buf, bufSize, "slli %s, %d", regNames[rd_rs1], imm7 & 0xF);
                else if (imm_high == 0x2)
                    snprintf(buf, bufSize, "srli %s, %d", regNames[rd_rs1], imm7 & 0xF);
                else if (imm_high == 0x4)
                    snprintf(buf, bufSize, "srai %s, %d", regNames[rd_rs1], imm7 & 0xF);
                else
                    snprintf(buf, bufSize, "unknown shift");
            }
            else if (funct3 == 0x4)
                snprintf(buf, bufSize, "ori %s, %d", regNames[rd_rs1], simm);
            else if (funct3 == 0x5)
                snprintf(buf, bufSize, "andi %s, %d", regNames[rd_rs1], simm);
            else if (funct3 == 0x6)
                snprintf(buf, bufSize, "xori %s, %d", regNames[rd_rs1], simm);
            else if (funct3 == 0x7)
                snprintf(buf, bufSize, "li %s, %d", regNames[rd_rs1], simm);
            else
                snprintf(buf, bufSize, "unknown I-type");

            break;
        }

        case 0x2: { // B-type (branch)
            uint8_t offset4_1 = (inst >> 12) & 0xF;  // bits 15–12 → offset[4:1]
            uint8_t rs2 = (inst >> 9) & 0x7;
            uint8_t rs1 = (inst >> 6) & 0x7;
            uint8_t funct3 = (inst >> 3) & 0x7;

            // Create full offset: shift left by 1, then sign-extend 5-bit to 16-bit
            int16_t offset = (offset4_1 << 1);
            if (offset & 0x10) offset |= 0xFFE0;  // Sign-extend 5 bits to 16 bits

            switch(funct3) {
                case 0x0:
                    snprintf(buf, bufSize, "beq %s, %s, PC+%d", regNames[rs1], regNames[rs2], offset);
                    break;
                case 0x1:
                    snprintf(buf, bufSize, "bne %s, %s, PC+%d", regNames[rs1], regNames[rs2], offset);
                    break;
                case 0x4:
                    snprintf(buf, bufSize, "blt %s, %s, PC+%d", regNames[rs1], regNames[rs2], offset);
                    break;
                case 0x5:
                    snprintf(buf, bufSize, "bge %s, %s, PC+%d", regNames[rs1], regNames[rs2], offset);
                    break;
                default:
                    snprintf(buf, bufSize, "unknown B-type");
                    break;
            }
            break;
        }

        case 0x4: { // L-type (load)
            uint8_t imm7 = (inst >> 9) & 0x7F;
            uint8_t rs1  = (inst >> 6) & 0x7;
            uint8_t funct3 = (inst >> 3) & 0x7;

            int16_t simm = (imm7 & 0x40) ? (imm7 | 0xFF80) : imm7; // sign-extend

            switch(funct3) {
                case 0x0:
                    snprintf(buf, bufSize, "lb a0, %d(%s)", simm, regNames[rs1]);
                    break;
                case 0x1:
                    snprintf(buf, bufSize, "lw a0, %d(%s)", simm, regNames[rs1]);
                    break;
                case 0x4:
                    snprintf(buf, bufSize, "lbu a0, %d(%s)", simm, regNames[rs1]);
                    break;
                default:
                    snprintf(buf, bufSize, "unknown L-type");
                    break;
            }
            break;
        }
        case 0x5: { // J-type
            uint8_t imm_hi = (inst >> 12) & 0xF;       // bits 15–12 = imm[9:6]
            uint8_t rd     = (inst >> 9) & 0x7;        // bits 11–9 = rd (used in link mode)
            uint8_t imm_mid = (inst >> 6) & 0x7;       // bits 8–6 = imm[5:3]
            uint8_t imm_lo  = (inst >> 3) & 0x7;       // bits 5–3 = imm[2:0]

            uint16_t imm10 = (imm_hi << 6) | (imm_mid << 3) | imm_lo;
            int16_t simm = imm10 << 1;
            if (simm & 0x200) simm |= 0xFC00; // sign-extend 10-bit

            uint8_t link = (rd != 0);  // if rd is used, it's a J (not JAL)

            if (link)
                snprintf(buf, bufSize, "j %s, PC+%d", regNames[rd], simm);
            else
                snprintf(buf, bufSize, "jal PC+%d", simm);

            break;
        }
        case 0x6: { // U-type (LUI / AUIPC)
            uint16_t imm_part = (inst >> 7) & 0x1FF;  // bits 15–7
            uint8_t flag = (inst >> 6) & 0x1;         // bit 6

            uint16_t imm = imm_part << 7;  // full 16-bit immediate

            if (flag == 0)
                snprintf(buf, bufSize, "lui a0, 0x%X", imm);
            else
                snprintf(buf, bufSize, "auipc a0, PC+0x%X", imm);
            break;
        }
        case 0x7: { // SYS-type (ECALL)
            snprintf(buf, bufSize, "ecall");
            break;
        }

        default:
            snprintf(buf, bufSize, "Unknown opcode");
            break;
    }
}

// -----------------------
// Instruction Execution
// -----------------------
//
// Executes the instruction 'inst' (a 16-bit word) by updating registers, memory, and PC.
// Returns 1 to continue simulation or 0 to terminate (if ecall 3 is executed).
int executeInstruction(uint16_t inst) {
    uint8_t opcode = inst & 0x7;
    int pcUpdated = 0; // flag: if instruction updated PC directly
    switch(opcode) {
        case 0x0: { // R-type
            uint8_t funct4 = (inst >> 12) & 0xF;
            uint8_t rs2    = (inst >> 9) & 0x7;
            uint8_t rd     = (inst >> 6) & 0x7;
            uint8_t funct3 = (inst >> 3) & 0x7;

            if (funct3 != 0x0) {
                printf("Unknown R-type funct3: 0x%X\n", funct3);
                break;
            }

            switch (funct4) {
                case 0x0: // add
                    regs[rd] = regs[rd] + regs[rs2];
                    break;
                case 0x1: // sub
                    regs[rd] = regs[rd] - regs[rs2];
                    break;
                case 0x2: // and
                    regs[rd] = regs[rd] & regs[rs2];
                    break;
                case 0x3: // or
                    regs[rd] = regs[rd] | regs[rs2];
                    break;
                case 0x4: // xor
                    regs[rd] = regs[rd] ^ regs[rs2];
                    break;
                case 0x5: // slt
                    regs[rd] = ((int16_t)regs[rd] < (int16_t)regs[rs2]) ? 1 : 0;
                    break;
                case 0x6: // sltu
                    regs[rd] = (regs[rd] < regs[rs2]) ? 1 : 0;
                    break;
                case 0x7: // sll
                    regs[rd] = regs[rd] << (regs[rs2] & 0xF);
                    break;
                case 0x8: // srl
                    regs[rd] = regs[rd] >> (regs[rs2] & 0xF);
                    break;
                case 0x9: // sra
                    regs[rd] = ((int16_t)regs[rd]) >> (regs[rs2] & 0xF);
                    break;
                case 0xA: // mv
                    regs[rd] = regs[rs2];
                    break;
                case 0xB: // jr
                    pc = regs[rs2];
                    return 1;
                case 0xC: // jalr
                {
                    uint16_t tmp = pc + 2;
                    pc = regs[rd] + regs[rs2];
                    regs[rd] = tmp;
                    return 1;
                }
                default:
                    printf("Unknown R-type funct4: 0x%X\n", funct4);
                    break;
            }
            break;
        }

        case 0x1: { // I-type
            uint8_t imm7   = (inst >> 9) & 0x7F;
            uint8_t rs1    = (inst >> 6) & 0x7;
            uint8_t funct3 = (inst >> 3) & 0x7;

            // Sign-extend imm7 to 16 bits
            int16_t imm = (imm7 & 0x40) ? (imm7 | 0xFF80) : imm7;

            switch (funct3) {
                case 0x0: // addi or li (li = addi x0, imm)
                    regs[6] = regs[rs1] + imm;
                    break;

                case 0x1: // slli (logical shift left)
                    regs[6] = regs[rs1] << (imm & 0xF);
                    break;

                case 0x2: // slti (signed)
                    regs[6] = ((int16_t)regs[rs1] < imm) ? 1 : 0;
                    break;

                case 0x3: // sltui (unsigned)
                    regs[6] = (regs[rs1] < (uint16_t)imm) ? 1 : 0;
                    break;

                case 0x4: // xori
                    regs[6] = regs[rs1] ^ imm;
                    break;

                case 0x5: // srli (logical right shift)
                    regs[6] = regs[rs1] >> (imm & 0xF);
                    break;

                case 0x6:
                    if (imm7 & 0x40)  // imm[6] == 1
                        regs[6] = ((int16_t)regs[rs1]) >> (imm & 0xF);  // srai
                    else
                        regs[6] = regs[rs1] | imm;  // ori
                    break;

                case 0x7: // andi
                    regs[6] = regs[rs1] & imm;
                    break;

                default:
                    printf("Unknown I-type funct3: 0x%X\n", funct3);
                    break;
            }
            break;
        }

        case 0x2: { // B-type (branch)
            uint8_t offset4_1 = (inst >> 12) & 0xF;
            uint8_t rs2 = (inst >> 9) & 0x7;
            uint8_t rs1 = (inst >> 6) & 0x7;
            uint8_t funct3 = (inst >> 3) & 0x7;

            int16_t offset = offset4_1 << 1;
            if (offset & 0x10) offset |= 0xFFE0;  // Sign-extend from bit 4

            int taken = 0;

            switch (funct3) {
                case 0x0: // beq
                    if (regs[rs1] == regs[rs2]) taken = 1;
                    break;

                case 0x1: // bne
                    if (regs[rs1] != regs[rs2]) taken = 1;
                    break;

                case 0x2: // blt (signed)
                    if ((int16_t)regs[rs1] < (int16_t)regs[rs2]) taken = 1;
                    break;

                case 0x3: // bge (signed)
                    if ((int16_t)regs[rs1] >= (int16_t)regs[rs2]) taken = 1;
                    break;

                case 0x4: // bz (branch if zero)
                    if (regs[rs1] == 0) taken = 1;
                    break;

                case 0x5: // bnz (branch if not zero)
                    if (regs[rs1] != 0) taken = 1;
                    break;

                case 0x6: // bltu (unsigned)
                    if (regs[rs1] < regs[rs2]) taken = 1;
                    break;

                case 0x7: // bgeu (unsigned)
                    if (regs[rs1] >= regs[rs2]) taken = 1;
                    break;

                default:
                    printf("Unknown B-type funct3: 0x%X\n", funct3);
                    break;
            }

            if (taken) {
                pc += offset;
                return 1;  // skip default pc += 2
            }

            break;  // falls through to default pc += 2 in main loop
        }


        case 0x4: { // L-type (load)
            uint8_t imm4   = (inst >> 12) & 0xF;
            uint8_t rs2    = (inst >> 9) & 0x7;  // base address
            uint8_t rd     = (inst >> 6) & 0x7;  // destination
            uint8_t funct3 = (inst >> 3) & 0x7;

            // Sign-extend 4-bit immediate
            int16_t offset = (imm4 & 0x8) ? (imm4 | 0xFFF0) : imm4;
            uint16_t addr = regs[rs2] + offset;

            switch (funct3) {
                case 0x0: { // lb (load byte, sign-extended)
                    int8_t val = *((int8_t*)&memory[addr]);
                    regs[rd] = val;
                    break;
                }
                case 0x1: { // lw (load word, 16 bits)
                    uint16_t val = *(uint16_t*)&memory[addr];
                    regs[rd] = val;
                    break;
                }
                case 0x2: { // lbu (load byte, zero-extended)
                    uint8_t val = memory[addr];
                    regs[rd] = val;
                    break;
                }
                default:
                    printf("Unknown L-type funct3: 0x%X\n", funct3);
                    break;
            }
            break;
        }

        case 0x5: { // J-type (j / jal)
            uint8_t link = (inst >> 15) & 0x1;
            uint8_t imm_high = (inst >> 9) & 0x3F;  // imm[9:4]
            uint8_t rd       = (inst >> 6) & 0x7;
            uint8_t imm_low  = (inst >> 3) & 0x7;   // imm[3:1]

            // Combine imm[9:1]
            uint16_t imm = (imm_high << 4) | imm_low;
            imm <<= 1;  // add imm[0] = 0 (all jumps are even)

            // Sign-extend 10-bit offset
            int16_t offset = (imm & 0x200) ? (imm | 0xFC00) : imm;

            if (link) {
                // jal: save PC+2 into rd
                regs[rd] = pc + 2;
            }

            pc += offset;
            return 1;  // prevent pc += 2 fallback
        }

        case 0x6: { // U-type (LUI / AUIPC)
            uint8_t flag      = (inst >> 15) & 0x1;
            uint8_t imm_hi6   = (inst >> 9) & 0x3F;  // imm[15:10]
            uint8_t rd        = (inst >> 6) & 0x7;
            uint8_t imm_mid3  = (inst >> 3) & 0x7;   // imm[9:7]

            // Build full 16-bit immediate: imm = {imm[15:7], 7'b0000000}
            uint16_t imm = ((imm_hi6 << 3) | imm_mid3) << 7;

            if (flag == 0) {
                // LUI
                regs[rd] = imm;
            } else {
                // AUIPC
                regs[rd] = pc + imm;
            }
            break;
        }

        case 0x7: { // SYS-type (ecall)
            uint16_t service = regs[0]; // syscall ID in a0

            switch (service) {
                case 1: { // Read String
                    char *buf = (char *)&mem[regs[1]];
                    int maxLen = regs[2];
                    fgets(buf, maxLen, stdin);
                    regs[0] = strlen(buf); // return actual length
                    break;
                }

                case 2: { // Read Integer
                    int value;
                    scanf("%d", &value);
                    regs[0] = value; // store in a0
                    break;
                }

                case 3: { // Print String
                    char *str = (char *)&mem[regs[0]];
                    printf("%s", str);
                    break;
                }

                case 4: { // Play tone
                    printf("Play tone: freq = %d Hz, duration = %d ms\n", regs[0], regs[1]);
                    break;
                }

                case 5: { // Set audio volume
                    printf("Set volume to %d\n", regs[0]);
                    break;
                }

                case 6: { // Stop audio
                    printf("Audio playback stopped\n");
                    break;
                }

                case 7: { // Read keyboard
                    int key = getchar(); // simple version
                    regs[0] = key;
                    regs[1] = (key != EOF); // 1 if key was read
                    break;
                }

                case 8: { // Dump registers
                    for (int i = 0; i < 8; ++i)
                        printf("r%d = %d\n", i, regs[i]);
                    break;
                }

                case 9: { // Memory dump
                    uint16_t addr = regs[0];
                    uint16_t count = regs[1];
                    for (int i = 0; i < count; ++i)
                        printf("mem[%04x] = %02x\n", addr + i, mem[addr + i]);
                    break;
                }

                case 10: { // Exit
                    exit(0);
                    break;
                }

                default:
                    printf("Unknown syscall: %d\n", service);
                    break;
            }

            break;
        }

        default:
            printf("Unknown instruction opcode 0x%X\n", opcode);
            break;
    }
    if(!pcUpdated)
        pc += 2; // default: move to next instruction
    return 1;
}

// -----------------------
// Memory Loading
// -----------------------
//
// Loads the binary machine code image from the specified file into simulated memory.
void loadMemoryFromFile(const char *filename) {
    FILE *fp = fopen(filename, "rb");
    if(!fp) {
        perror("Error opening binary file");
        exit(1);
    }
    size_t n = fread(memory, 1, MEM_SIZE, fp);
    fclose(fp);
    printf("Loaded %zu bytes into memory\n", n);
}

// -----------------------
// Main Simulation Loop
// -----------------------
int main(int argc, char **argv) {
    if(argc != 2) {
        fprintf(stderr, "Usage: %s <machine_code_file>\n", argv[0]);
        exit(1);
    }
    loadMemoryFromFile(argv[1]);
    memset(regs, 0, sizeof(regs)); // initialize registers to 0
    pc = 0;  // starting at address 0
    char disasmBuf[128];
    while(pc < MEM_SIZE) {
        // Fetch a 16-bit instruction from memory (little-endian)
        uint16_t inst = memory[pc] | (memory[pc+1] << 8);
        disassemble(inst, pc, disasmBuf, sizeof(disasmBuf));
        printf("0x%04X: %04X    %s\n", pc, inst, disasmBuf);
        if(!executeInstruction(inst))
            break;
        // Terminate if PC goes out of bounds
        if(pc >= MEM_SIZE) break;
    }
    return 0;
}
