#ifndef ECALLS_H
#define ECALLS_H

#include <cstdint>
#include <string>
#include <vector>
#include <map>
#include"Graphics.h"
// Forward declarations
struct DecodedInstruction;
class Registers;
class Memory;
class Graphics;

// ECALL Service Numbers (based on ZX16 specification)
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

// ECALL execution context - contains all data needed for ECALL execution
struct EcallContext {
    uint16_t service_number;
    uint16_t a0;  // x6 - first argument/return value
    uint16_t a1;  // x7 - second argument
    uint16_t a2;  // stack-based if needed
    uint16_t a3;  // stack-based if needed

    // Constructor
    EcallContext(uint16_t service, uint16_t arg0 = 0, uint16_t arg1 = 0)
        : service_number(service), a0(arg0), a1(arg1), a2(0), a3(0) {}
};

// ECALL result structure
struct EcallResult {
    bool success;
    uint16_t return_value;  // Value to store in a0 (x6)
    bool should_halt;
    std::string error_message;

    EcallResult() : success(true), return_value(0), should_halt(false) {}
    EcallResult(bool ok, uint16_t ret_val = 0, bool halt = false, const std::string& err = "")
        : success(ok), return_value(ret_val), should_halt(halt), error_message(err) {}
};

class Ecalls {
public:
    ~Ecalls();

    // Main ECALL handler
    void handle(const DecodedInstruction& instr, Registers& regs, Memory& mem, bool& halted, Graphics& gfx);

    // Create ECALL context from current register state
    EcallContext createContext(uint16_t service, const Registers& regs);

    // Apply ECALL result to registers
    void applyResult(const EcallResult& result, Registers& regs, bool& halted);

private:
    // Individual ECALL service handlers
    EcallResult readString(const EcallContext& ctx, Memory& mem);
    EcallResult readInteger(const EcallContext& ctx, Memory& mem);

   // std::string readStringFromMemory(const Memory & mem, uint16_t addr);
     std::string readStringFromMemory(const Memory &mem, uint16_t addr);

    EcallResult printString(const EcallContext& ctx, Memory& mem);


    EcallResult playTone(const EcallContext& ctx, Memory& mem, Graphics& gfx);
    EcallResult setAudioVolume(const EcallContext& ctx, Memory& mem, Graphics& gfx);
    EcallResult stopAudio(const EcallContext& ctx, Memory& mem, Graphics& gfx);

    void cleanup();

    EcallResult readKeyboard(const EcallContext& ctx, Memory& mem, Graphics& gfx);
    EcallResult registersDump(const EcallContext& ctx, const Registers& regs);
    EcallResult memoryDump(const EcallContext& ctx, Memory& mem);
    EcallResult programExit(const EcallContext& ctx);

    // Utility functions
    std::string readStringFromMemory(Memory& mem, uint16_t addr, uint16_t maxlen = 1024);
    void writeStringToMemory(Memory& mem, uint16_t addr, const std::string& str, uint16_t maxlen);
    bool isValidMemoryAddress(uint16_t addr);
    std::string getServiceName(uint16_t service);

    // ECALL statistics (optional)
    std::map<uint16_t, uint32_t> ecall_counts;

public:
    // Debug functions
    void printEcallStats();
    void resetEcallStats();
};

#endif // ECALLS_H