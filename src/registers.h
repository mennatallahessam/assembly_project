#ifndef REGISTERS_H
#define REGISTERS_H

#include <cstdint>
#include <array>
#include <string>
#include <stdexcept>

class Registers {
private:
    static const int NUM_REGISTERS = 8;  // ZX16 has only 8 registers (x0-x7)
    std::array<uint16_t, NUM_REGISTERS> regs;  // 16-bit registers for ZX16

    // ZX16 register names
    static const std::array<std::string, NUM_REGISTERS> register_names;

public:
    // Constructor
    Registers();

    // Access operators with bounds checking
    uint16_t& operator[](int idx);
    uint16_t operator[](int idx) const;

    // Register access by name (for debugging/assembly)
    uint16_t getRegister(const std::string& name) const;
    void setRegister(const std::string& name, uint16_t value);

    // Special register access
    uint16_t getPC() const { return pc; }
    void setPC(uint16_t value) { pc = value; }
    void incrementPC(uint16_t increment = 2) { pc += increment; } // ZX16 uses 2-byte instructions

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
    uint16_t pc; // 16-bit Program Counter for ZX16

    // Helper function to validate register index
    void validateIndex(int idx) const;
};

#endif // REGISTERS_H