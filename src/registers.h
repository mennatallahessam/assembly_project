#ifndef REGISTERS_H
#define REGISTERS_H

#include <cstdint>
#include <array>
#include <string>
#include <stdexcept>

class Registers {
private:
    static const int NUM_REGISTERS = 32;
    std::array<uint32_t, NUM_REGISTERS> regs;
    
    // ABI register names for ZX16 (RISC-V style)
    static const std::array<std::string, NUM_REGISTERS> register_names;
    
public:
    // Constructor
    Registers();
    
    // Access operators with bounds checking
    uint32_t& operator[](int idx);
    uint32_t operator[](int idx) const;
    
    // Register access by name (for debugging/assembly)
    uint32_t getRegister(const std::string& name) const;
    void setRegister(const std::string& name, uint32_t value);
    
    // Special register access (commonly used in ZX16)
    uint32_t getPC() const { return pc; }
    void setPC(uint32_t value) { pc = value; }
    void incrementPC(uint32_t increment = 2) { pc += increment; } // ZX16 likely uses 2-byte instructions
    
    // Utility functions
    void reset();
    void dump() const;
    std::string getRegisterName(int idx) const;
    int getRegisterIndex(const std::string& name) const;
    
    // Exception class
    class RegisterException : public std::exception {
    private:
        std::string message;
    public:
        RegisterException(const std::string& msg) : message(msg) {}
        const char* what() const noexcept override { return message.c_str(); }
    };
    
private:
    uint32_t pc; // Program Counter (separate from general registers)
    
    // Helper function to validate register index
    void validateIndex(int idx) const;
};

#endif // REGISTERS_H
