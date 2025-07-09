// ================ ecalls.h ================
#ifndef ECALLS_H
#define ECALLS_H

#include <cstdint>
#include <string>
#include <map>
#include "graphics.h"

struct DecodedInstruction;
class Registers;
class Memory;

enum EcallService {
    ECALL_READ_STRING = 1,
    ECALL_READ_INTEGER = 2,
    ECALL_PRINT_STRING = 3,
    ECALL_PLAY_TONE = 4,
    ECALL_SET_AUDIO_VOLUME = 5,
    ECALL_STOP_AUDIO = 6,
    ECALL_READ_KEYBOARD = 7,
    ECALL_REGISTERS_DUMP = 8,
    ECALL_MEMORY_DUMP = 9,
    ECALL_PROGRAM_EXIT = 10
};

struct EcallContext {
    uint16_t service_number;
    uint16_t a0, a1, a2, a3;
    EcallContext(uint16_t service, uint16_t arg0 = 0, uint16_t arg1 = 0) 
        : service_number(service), a0(arg0), a1(arg1), a2(0), a3(0) {}
};

struct EcallResult {
    bool success;
    uint16_t return_value;
    bool should_halt;
    std::string error_message;
    EcallResult(bool ok = true, uint16_t ret = 0, bool halt = false, const std::string& err = "")
        : success(ok), return_value(ret), should_halt(halt), error_message(err) {}
};

class Ecalls {
public:
    void handle(const DecodedInstruction& instr, Registers& regs, Memory& mem, bool& halted, graphics& gfx);
    
    // Audio-related ECALLs
    EcallResult playTone(const EcallContext& ctx, Memory& mem, graphics& gfx);
    EcallResult setAudioVolume(const EcallContext& ctx, Memory& mem, graphics& gfx);
    EcallResult stopAudio(const EcallContext& ctx, Memory& mem, graphics& gfx);
    
    // I/O ECALLs
    EcallResult readString(const EcallContext& ctx, Memory& mem, graphics& gfx);
    EcallResult readInteger(const EcallContext& ctx, Memory& mem, graphics& gfx);
    EcallResult printString(const EcallContext& ctx, Memory& mem, graphics& gfx);
    EcallResult readKeyboard(const EcallContext& ctx, Memory& mem, graphics& gfx);
    
    // Debug ECALLs
    EcallResult registersDump(const EcallContext& ctx, Memory& mem, graphics& gfx);
    EcallResult memoryDump(const EcallContext& ctx, Memory& mem, graphics& gfx);
    EcallResult programExit(const EcallContext& ctx, Memory& mem, graphics& gfx);

private:
    std::map<uint16_t, uint32_t> ecall_counts;
    
    // Utility methods
    std::string readNullTerminatedString(Memory& mem, uint16_t address);
    void writeNullTerminatedString(Memory& mem, uint16_t address, const std::string& str);
    bool isValidMemoryRange(Memory& mem, uint16_t start, uint16_t length);
    void logEcallUsage(uint16_t service_number);
};

#endif

