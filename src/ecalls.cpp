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
            // Special handling for readKeyboard - need to set both a0 and a1
            if (result.success) {
                regs[6] = result.return_value;  // x6 (a0) = key code
                regs[7] = (result.return_value != 0) ? 1 : 0;  // x7 (a1) = key pressed flag
            }
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

    // Apply result to processor state (but skip for readKeyboard since we handled it above)
    if (service != 7) {
        applyResult(result, regs, halted);
    } else {
        // For readKeyboard, we only need to handle the halt flag
        if (result.should_halt) {
            halted = true;
        }
    }

    if (!result.success) {
        std::cerr << "[ECALL ERROR] " << result.error_message << std::endl;
    }
}
EcallContext Ecalls::createContext(uint16_t service, const Registers& regs) {
    // Debug register values
   /* std::cout << "[DEBUG] Creating context for service " << service << std::endl;
    std::cout << "[DEBUG] Register x6 (a0): 0x" << std::hex << regs[6] << std::endl;
    std::cout << "[DEBUG] Register x7 (a1): 0x" << std::hex << regs[7] << std::endl;
*/
    // Check if there's an endianness issue
    uint16_t a0_val = regs[6];
    uint16_t a1_val = regs[7];

    // Check for potential byte-swapped values
    uint16_t a0_swapped = ((a0_val & 0xFF) << 8) | ((a0_val >> 8) & 0xFF);
    //std::cout << "[DEBUG] a0 byte-swapped would be: 0x" << std::hex << a0_swapped << std::endl;

    EcallContext ctx(service, a0_val, a1_val);
    return ctx;
}
void Ecalls::applyResult(const EcallResult& result, Registers& regs, bool& halted) {
    if (result.success) {
        // Store return value in a0 (x6)
        regs[6] = result.return_value;
    }

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

    // Check if this looks like a small integer rather than a string address
    if (addr < 0x100) {  // Values < 256 are probably integers, not addresses
        // Print as integer
        int16_t signed_val = static_cast<int16_t>(addr);
        std::cout << std::dec << signed_val << std::endl;
        return EcallResult(true, 1, false);
    }

    // Try to read as string address
    std::string str = readStringFromMemory(mem, addr);

    if (!str.empty()) {
        std::cout << str << std::endl;
        return EcallResult(true, str.length(), false);
    } else {
        // Fallback to integer printing
        int16_t signed_val = static_cast<int16_t>(addr);
        std::cout << std::dec << signed_val << std::endl;
        return EcallResult(true, 1, false);
    }
}
// 2. ISSUE: Memory access endianness or alignment
// The problem might be in how 16-bit addresses are stored/retrieved

// SOLUTION 2: Improved readStringFromMemory with better address handling
std::string Ecalls::readStringFromMemory(const Memory& mem, uint16_t addr) {
    std::string result;

    // Ensure we're starting at the correct address
    //std::cout << "[DEBUG] readStringFromMemory: Starting at 0x" << std::hex << addr << std::endl;

    // Check if we need to handle alignment issues
    if (addr % 2 != 0) {
        std::cout << "[DEBUG] Warning: Unaligned address 0x" << std::hex << addr << std::endl;
    }

    try {
        // Read bytes sequentially from the exact address
        for (int i = 0; i < 100; i++) {
            uint16_t current_addr = addr + i;

            if (!isValidMemoryAddress(current_addr)) {
                std::cout << "[DEBUG] Invalid address reached: 0x" << std::hex << current_addr << std::endl;
                break;
            }

            uint8_t byte = mem.load8(current_addr);

          //  std::cout << "[DEBUG] Address 0x" << std::hex << current_addr
            //          << " -> 0x" << std::setw(2) << std::setfill('0') << (int)byte;

            if (byte == 0) {
                std::cout << " (NULL)" << std::endl;
                break;
            }

            if (byte >= 32 && byte <= 126) {
                result += static_cast<char>(byte);
                std::cout << " ('" << static_cast<char>(byte) << "')" << std::endl;
            } else {
                std::cout << " (non-printable)" << std::endl;
                // For debugging, you might want to include non-printable chars
                // result += '?';
            }
        }
    } catch (const std::exception& e) {
        std::cout << "[DEBUG] Exception: " << e.what() << std::endl;
        return "";
    }

    return result;
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
    uint16_t key = 0;
    bool keyPressed = false;

    // Check if there's input available without blocking
    std::cout << "Enter key (or press Enter for no key): ";
    std::string input;
    std::getline(std::cin, input);

    if (!input.empty()) {
        // Get the first character and convert to key code
        key = static_cast<uint16_t>(input[0]);
        keyPressed = true;
    }

    // According to the service specification:
    // x6 (a0) = the key code
    // x7 (a1) = 1 if a key is pressed, 0 if nothing is pressed

    // We'll handle setting both registers in a special way for this ecall
    return EcallResult(true, key, false);
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
