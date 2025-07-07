#include "ecalls.h"
#include "Decoder.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <limits>

#include "memory.h"
#include "registers.h"

void Ecalls::handle(const DecodedInstruction& instr, Registers& regs, Memory& mem, bool& halted, graphics& gfx) {
    // Extract service number from the decoded instruction
    uint16_t service = instr.syscall_num;

    // Update statistics
    ecall_counts[service]++;

    // Create execution context
    EcallContext ctx = createContext(service, regs);

    // Execute the appropriate service
    EcallResult result;

    switch (service) {
        case 1:
            result = readString(ctx, mem);
        break;

        case 2:
            result = readInteger(ctx, mem);
        break;

        case 3:
            result = printString(ctx, mem);
        break;

        case 4:
            result = playTone(ctx, mem, gfx);
        break;

        case 5:
            result = setAudioVolume(ctx, mem, gfx);
        break;

        case 6:
            result = stopAudio(ctx, mem, gfx);
        break;

        case 7:
            result = readKeyboard(ctx, mem, gfx);
        break;

        case 8:
            result = registersDump(ctx, regs);
        break;

        case 9:
            result = memoryDump(ctx, mem);
        break;

        case 10:
            result = programExit(ctx);
        break;

        default:
            std::cerr << "[ECALL ERROR] Unknown service: " << service << std::endl;
            result = EcallResult(false, 0, true, "Unknown ECALL service");
    }

    // Apply result to processor state
    applyResult(result, regs, halted);

    if (!result.success) {
        std::cerr << "[ECALL ERROR] " << result.error_message << std::endl;
    }
}

EcallContext Ecalls::createContext(uint16_t service, const Registers& regs) {
    // ZX16 ABI: a0 = x6, a1 = x7
    EcallContext ctx(service, regs[6], regs[7]);
    return ctx;
}

void Ecalls::applyResult(const EcallResult& result, Registers& regs, bool& halted) {
   /* if (result.success) {
        // Store return value in a0 (x6)
        regs[6] = result.return_value;
    }*/

    if (result.should_halt) {
        halted = true;
    }
}

// =============================================================================
// ECALL SERVICE IMPLEMENTATIONS
// =============================================================================

EcallResult Ecalls::readString(const EcallContext& ctx, Memory& mem) {
    uint16_t addr = ctx.a0;     // Address to store string
    uint16_t maxlen = ctx.a1;   // Maximum length

    if (!isValidMemoryAddress(addr)) {
        return EcallResult(false, 0, false, "Invalid memory address for string storage");
    }

    std::string input;
    std::cout << "Enter string: ";
    std::getline(std::cin, input);

    // Limit input length
    if (input.length() >= maxlen) {
        input.resize(maxlen - 1); // Leave room for null terminator
    }

    // Write string to memory
    writeStringToMemory(mem, addr, input, maxlen);

    return EcallResult(true, input.length(), false);
}

EcallResult Ecalls::readInteger(const EcallContext& ctx, Memory& mem) {
    int32_t val;
    std::cout << "Enter integer: ";
    std::cin >> val;

    // Handle input validation
    if (std::cin.fail()) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        return EcallResult(false, 0, false, "Invalid integer input");
    }

    // Clear the input buffer
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    // Clamp to 16-bit signed range
    if (val > 32767) val = 32767;
    if (val < -32768) val = -32768;

    uint16_t result = static_cast<uint16_t>(static_cast<int16_t>(val));

    return EcallResult(true, result, false);
}

EcallResult Ecalls::printString(const EcallContext& ctx, Memory& mem) {
    uint16_t addr = ctx.a0;

    // Treat small values as numbers to print directly in decimal
    if (addr < 1000 || addr > 32767) {  // addr > 32767 means negative in 16-bit signed
        int16_t signed_val = static_cast<int16_t>(addr);
        std::cout << std::dec << static_cast<int>(signed_val) << std::endl;
        return EcallResult(true, 1, false);
    }



    if (!isValidMemoryAddress(addr)) {
        return EcallResult(false, 0, false, "Invalid memory address for string");
    }

    // Try to read the string from memory
    std::string str = readStringFromMemory(mem, addr);

    // If string is empty, try reading as a single character or number
    if (str.empty()) {
        uint16_t value = mem.load16(addr);
        int16_t val = static_cast<int16_t>(value);

        // Debug output
        std::cout << "Debug: loaded uint16_t = " << value
                  << ", cast to int16_t = " << val << std::endl;

        std::cout << std::dec << static_cast<int>(val) << std::endl;
        return EcallResult(true, 1, false);
    }


    // Print the string
    std::cout << str;
    return EcallResult(true, str.length(), false);
}

EcallResult Ecalls::playTone(const EcallContext& ctx, Memory& mem, graphics& gfx) {
   /* uint16_t freq = ctx.a0;
    uint16_t duration = ctx.a1;

    gfx.playSound(freq, duration); // implement this in graphics*/
    return EcallResult(true, 0, false);
}


EcallResult Ecalls::setAudioVolume(const EcallContext& ctx, Memory& mem, graphics& gfx) {
   /* uint16_t volume = ctx.a0;

    if (volume > 255) {
        return EcallResult(false, 0, false, "Volume must be 0-255");
    }

    gfx.setVolume(volume); // implement in graphics class*/
    return EcallResult(true, 0, false);
}


EcallResult Ecalls::stopAudio(const EcallContext& ctx, Memory& mem, graphics& gfx) {
 //   gfx.stopSound(); // implement in graphics class
    return EcallResult(true, 0, false);
}


EcallResult Ecalls::readKeyboard(const EcallContext& ctx, Memory& mem, graphics& gfx) {
   // uint16_t key = gfx.getKeyPressed(); // implement in graphics class
    return EcallResult(true, 0, false);
}


EcallResult Ecalls::registersDump(const EcallContext& ctx, const Registers& regs) {
    std::cout << "\n=== REGISTER DUMP ===" << std::endl;

    const char* reg_names[] = {"t0", "ra", "sp", "s0", "s1", "t1", "a0", "a1"};

    for (int i = 0; i < 8; i++) {
        std::cout << "x" << i << " (" << reg_names[i] << "): 0x"
                  << std::hex << std::setw(4) << std::setfill('0') << regs[i]
                  << " (" << std::dec << static_cast<int16_t>(regs[i]) << ")" << std::endl;
    }

    std::cout << "===================" << std::endl;
    return EcallResult(true, 0, false);
}

EcallResult Ecalls::memoryDump(const EcallContext& ctx, Memory& mem) {
    uint16_t start_addr = ctx.a0;
    uint16_t length = ctx.a1;

    if (length == 0) length = 64; // Default dump size

    std::cout << "\n=== MEMORY DUMP ===" << std::endl;
    std::cout << "Address: 0x" << std::hex << start_addr
              << ", Length: " << std::dec << length << " bytes" << std::endl;

    for (uint16_t i = 0; i < length; i += 16) {
        std::cout << std::hex << std::setw(4) << std::setfill('0') << (start_addr + i) << ": ";

        // Print hex bytes
        for (int j = 0; j < 16 && (i + j) < length; j++) {
            if (isValidMemoryAddress(start_addr + i + j)) {
                uint8_t byte = mem.readByte(start_addr + i + j);
                std::cout << std::hex << std::setw(2) << std::setfill('0') << (int)byte << " ";
            } else {
                std::cout << "?? ";
            }
        }

        // Print ASCII representation
        std::cout << " |";
        for (int j = 0; j < 16 && (i + j) < length; j++) {
            if (isValidMemoryAddress(start_addr + i + j)) {
                uint8_t byte = mem.readByte(start_addr + i + j);
                char c = (byte >= 32 && byte <= 126) ? byte : '.';
                std::cout << c;
            } else {
                std::cout << '?';
            }
        }
        std::cout << "|" << std::endl;
    }

    std::cout << "===================" << std::endl;
    return EcallResult(true, 0, false);
}

EcallResult Ecalls::programExit(const EcallContext& ctx) {
    uint16_t exit_code = ctx.a0;
    return EcallResult(true, exit_code, true);
}

// =============================================================================
// UTILITY FUNCTIONS
// =============================================================================

std::string Ecalls::readStringFromMemory(Memory& mem, uint16_t addr, uint16_t maxlen) {
    std::string result;
    result.reserve(maxlen);

    for (uint16_t i = 0; i < maxlen; i++) {
        if (!isValidMemoryAddress(addr + i)) {
            break;
        }

        uint8_t byte = mem.readByte(addr + i);
        if (byte == 0) {
            break; // Null terminator
        }

        result += static_cast<char>(byte);
    }

    return result;
}

void Ecalls::writeStringToMemory(Memory& mem, uint16_t addr, const std::string& str, uint16_t maxlen) {
    uint16_t len = std::min(static_cast<uint16_t>(str.length()), static_cast<uint16_t>(maxlen - 1));

    for (uint16_t i = 0; i < len; i++) {
        if (isValidMemoryAddress(addr + i)) {
            mem.writeByte(addr + i, str[i]);
        }
    }

    // Write null terminator
    if (isValidMemoryAddress(addr + len)) {
        mem.writeByte(addr + len, 0);
    }
}

bool Ecalls::isValidMemoryAddress(uint16_t addr) {
    // Check if address is in valid memory range
    // For now, allow all addressable memory except reserved interrupt vectors
    // Interrupt vectors: 0x0000-0x001F (but allow for testing)
    // RAM/ROM: 0x0020-0xEFFF
    // MMIO: 0xF000-0xFFFF

    // Temporarily allow all memory for testing
    return addr <= 0xFFFF;

    // Original restrictive version:
    // return (addr >= 0x0020 && addr <= 0xEFFF) || (addr >= 0xF000 && addr <= 0xFFFF);
}

std::string Ecalls::getServiceName(uint16_t service) {
    switch (service) {
        case ECALL_READ_STRING: return "READ_STRING";
        case ECALL_READ_INTEGER: return "READ_INTEGER";
        case ECALL_PRINT_STRING: return "PRINT_STRING";
        case ECALL_PLAY_TONE: return "PLAY_TONE";
        case ECALL_SET_AUDIO_VOLUME: return "SET_AUDIO_VOLUME";
        case ECALL_STOP_AUDIO: return "STOP_AUDIO";
        case ECALL_READ_KEYBOARD: return "READ_KEYBOARD";
        case ECALL_REGISTERS_DUMP: return "REGISTERS_DUMP";
        case ECALL_MEMORY_DUMP: return "MEMORY_DUMP";
        case ECALL_PROGRAM_EXIT: return "PROGRAM_EXIT";
        default: return "UNKNOWN";
    }
}

void Ecalls::printEcallStats() {
    std::cout << "\n=== ECALL STATISTICS ===" << std::endl;
    for (const auto& pair : ecall_counts) {
        std::cout << getServiceName(pair.first) << " (" << pair.first << "): "
                  << pair.second << " calls" << std::endl;
    }
    std::cout << "========================" << std::endl;
}

void Ecalls::resetEcallStats() {
    ecall_counts.clear();
}
