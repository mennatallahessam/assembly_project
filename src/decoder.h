#ifndef DECODER_H
#define DECODER_H

#include <cstdint>

struct DecodedInstr {
    uint8_t opcode;     // Operation code
    uint8_t rd;         // Destination register
    uint8_t rs1;        // Source register 1
    uint8_t rs2;        // Source register 2
    int16_t immediate;  // Immediate value (if applicable)
};

class decoder {
public:
    DecodedInstr decode(uint16_t instr);
};

#endif // DECODER_H
