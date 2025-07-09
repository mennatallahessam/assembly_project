#include "ecalls.h"
#include "Decoder.h"
#include "memory.h"
#include "registers.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <limits>

void Ecalls::handle(const DecodedInstruction& instr, Registers& regs, Memory& mem, bool& halted, graphics& gfx) {
    uint16_t service = regs.get(6); // x6 register holds service number
    uint16_t a0 = regs.get(7);      // x7 register holds first argument
    uint16_t a1 = regs.get(8);      // x8 register holds second argument
    
    EcallContext ctx(service, a0, a1);
    EcallResult result;
    
    logEcallUsage(service);
    
    switch (service) {
        case ECALL_READ_STRING:
            result = readString(ctx, mem, gfx);
            break;
        case ECALL_READ_INTEGER:
            result = readInteger(ctx, mem, gfx);
            break;
        case ECALL_PRINT_STRING:
            result = printString(ctx, mem, gfx);
            break;
        case ECALL_PLAY_TONE:
            result = playTone(ctx, mem, gfx);
            break;
        case ECALL_SET_AUDIO_VOLUME:
            result = setAudioVolume(ctx, mem, gfx);
            break;
        case ECALL_STOP_AUDIO:
            result = stopAudio(ctx, mem, gfx);
            break;
        case ECALL_READ_KEYBOARD:
            result = readKeyboard(ctx, mem, gfx);
            break;
        case ECALL_REGISTERS_DUMP:
            result = registersDump(ctx, mem, gfx);
            break;
        case ECALL_MEMORY_DUMP:
            result = memoryDump(ctx, mem, gfx);
            break;
        case ECALL_PROGRAM_EXIT:
            result = programExit(ctx, mem, gfx);
            break;
        default:
            result = EcallResult(false, 0, false, "Unknown ECALL service: " + std::to_string(service));
            break;
    }
    
    // Set return value in x6
    regs.set(6, result.return_value);
    
    // Handle errors
    if (!result.success) {
        std::cerr << "ECALL Error [" << service << "]: " << result.error_message << std::endl;
    }
    
    // Handle halt condition
    if (result.should_halt) {
        halted = true;
    }
}

// ================ Audio ECALL Implementations ================

EcallResult Ecalls::playTone(const EcallContext& ctx, Memory& mem, graphics& gfx) {
    if (!gfx.isAudioAvailable()) {
        return EcallResult(false, 0, false, "Audio system not initialized");
    }

    uint16_t freq = ctx.a0;
    uint16_t duration = ctx.a1;

    // Validate frequency range (human hearing range)
    if (freq < 20 || freq > 20000) {
        return EcallResult(false, 0, false, "Frequency must be 20-20000 Hz");
    }

    // Validate duration (1ms to ~65 seconds)
    if (duration == 0 || duration > 65535) {
        return EcallResult(false, 0, false, "Duration must be 1-65535 ms");
    }

    // Actual tone generation
    if (!gfx.playTone(freq, duration)) {
        return EcallResult(false, 0, false, "Failed to play tone");
    }

    return EcallResult(true, 0, false);
}

EcallResult Ecalls::setAudioVolume(const EcallContext& ctx, Memory& mem, graphics& gfx) {
    if (!gfx.isAudioAvailable()) {
        return EcallResult(false, 0, false, "Audio system not initialized");
    }

    uint16_t volume = ctx.a0;

    // Validate volume range (0-255)
    if (volume > 255) {
        return EcallResult(false, 0, false, "Volume must be 0-255");
    }

    // Set system volume
    gfx.setVolume(static_cast<uint8_t>(volume));

    // Return the actual volume set for confirmation
    return EcallResult(true, volume, false);
}

EcallResult Ecalls::stopAudio(const EcallContext& ctx, Memory& mem, graphics& gfx) {
    if (!gfx.isAudioAvailable()) {
        return EcallResult(false, 0, false, "Audio system not initialized");
    }

    gfx.stopAudio();
    return EcallResult(true, 0, false);
}

// ================ I/O ECALL Implementations ================

EcallResult Ecalls::readString(const EcallContext& ctx, Memory& mem, graphics& gfx) {
    uint16_t buffer_addr = ctx.a0;
    uint16_t max_length = ctx.a1;
    
    if (max_length == 0) {
        return EcallResult(false, 0, false, "Invalid buffer size");
    }
    
    if (!isValidMemoryRange(mem, buffer_addr, max_length)) {
        return EcallResult(false, 0, false, "Invalid memory range");
    }
    
    std::string input;
    std::cout << "Enter string: ";
    std::getline(std::cin, input);
    
    // Truncate if necessary
    if (input.length() >= max_length) {
        input = input.substr(0, max_length - 1);
    }
    
    writeNullTerminatedString(mem, buffer_addr, input);
    return EcallResult(true, input.length(), false);
}

EcallResult Ecalls::readInteger(const EcallContext& ctx, Memory& mem, graphics& gfx) {
    std::cout << "Enter integer: ";
    std::string input;
    std::getline(std::cin, input);
    
    try {
        int value = std::stoi(input);
        if (value < std::numeric_limits<int16_t>::min() || 
            value > std::numeric_limits<int16_t>::max()) {
            return EcallResult(false, 0, false, "Integer out of range");
        }
        return EcallResult(true, static_cast<uint16_t>(value), false);
    } catch (const std::exception&) {
        return EcallResult(false, 0, false, "Invalid integer input");
    }
}

EcallResult Ecalls::printString(const EcallContext& ctx, Memory& mem, graphics& gfx) {
    uint16_t string_addr = ctx.a0;
    
    try {
        std::string str = readNullTerminatedString(mem, string_addr);
        std::cout << str << std::flush;
        return EcallResult(true, str.length(), false);
    } catch (const std::exception& e) {
        return EcallResult(false, 0, false, "Failed to read string: " + std::string(e.what()));
    }
}

EcallResult Ecalls::readKeyboard(const EcallContext& ctx, Memory& mem, graphics& gfx) {
    // Simple keyboard input - return ASCII value of pressed key
    std::cout << "Press a key: ";
    char key;
    std::cin.get(key);
    std::cin.ignore(); // Clear the newline
    
    return EcallResult(true, static_cast<uint16_t>(key), false);
}

// ================ Debug ECALL Implementations ================

EcallResult Ecalls::registersDump(const EcallContext& ctx, Memory& mem, graphics& gfx) {
    std::cout << "\n=== REGISTER DUMP ===" << std::endl;
    // This would need access to the Registers object
    // For now, just acknowledge the call
    std::cout << "Register dump requested" << std::endl;
    return EcallResult(true, 0, false);
}

EcallResult Ecalls::memoryDump(const EcallContext& ctx, Memory& mem, graphics& gfx) {
    uint16_t start_addr = ctx.a0;
    uint16_t length = ctx.a1;
    
    if (length == 0) length = 16; // Default dump size
    
    std::cout << "\n=== MEMORY DUMP ===" << std::endl;
    std::cout << "Address  | Data" << std::endl;
    std::cout << "---------|--------" << std::endl;
    
    for (uint16_t i = 0; i < length; i++) {
        uint16_t addr = start_addr + i;
        try {
            uint8_t data = mem.getByte(addr);
            std::cout << std::hex << std::setw(4) << std::setfill('0') << addr 
                      << "     | " << std::setw(2) << static_cast<int>(data) << std::endl;
        } catch (const std::exception&) {
            std::cout << std::hex << std::setw(4) << std::setfill('0') << addr 
                      << "     | ??" << std::endl;
        }
    }
    
    return EcallResult(true, length, false);
}

EcallResult Ecalls::programExit(const EcallContext& ctx, Memory& mem, graphics& gfx) {
    uint16_t exit_code = ctx.a0;
    std::cout << "\nProgram exited with code: " << exit_code << std::endl;
    return EcallResult(true, exit_code, true); // Signal halt
}

// ================ Utility Methods ================

std::string Ecalls::readNullTerminatedString(Memory& mem, uint16_t address) {
    std::string result;
    uint16_t current_addr = address;
    
    while (current_addr < 0xFFFF) {
        uint8_t byte = mem.getByte(current_addr);
        if (byte == 0) break; // Null terminator
        result += static_cast<char>(byte);
        current_addr++;
        
        // Prevent infinite loops
        if (result.length() > 1024) {
            throw std::runtime_error("String too long or missing null terminator");
        }
    }
    
    return result;
}

void Ecalls::writeNullTerminatedString(Memory& mem, uint16_t address, const std::string& str) {
    uint16_t current_addr = address;
    
    for (char c : str) {
        mem.setByte(current_addr++, static_cast<uint8_t>(c));
    }
    
    // Add null terminator
    mem.setByte(current_addr, 0);
}

bool Ecalls::isValidMemoryRange(Memory& mem, uint16_t start, uint16_t length) {
    // Check for overflow
    if (start + length < start) return false;
    
    // Check if end address is within bounds
    if (start + length > 0xFFFF) return false;
    
    return true;
}

void Ecalls::logEcallUsage(uint16_t service_number) {
    ecall_counts[service_number]++;
    
    // Optional: Log frequently used ECALLs
    if (ecall_counts[service_number] % 100 == 0) {
        std::cout << "[DEBUG] ECALL " << service_number 
                  << " used " << ecall_counts[service_number] << " times" << std::endl;
    }
}
