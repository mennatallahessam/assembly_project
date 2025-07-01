#ifndef MEMORY_H
#define MEMORY_H

#include <cstdint>
#include <stdexcept>

class Memory {
private:
    static const uint32_t MEMORY_SIZE = 65536; // 64KB for embedded system
    uint8_t data[MEMORY_SIZE];
    
    // Helper function to check bounds
    void checkBounds(uint32_t addr, uint32_t size) const;
    
public:
    // Constructor
    Memory();
    
    // Byte operations (always aligned)
    uint8_t readByte(uint32_t addr) const;
    void writeByte(uint32_t addr, uint8_t val);
    
    // Halfword operations (16-bit, must be 2-byte aligned)
    uint16_t readHalfWord(uint32_t addr) const;
    void writeHalfWord(uint32_t addr, uint16_t val);
    
    // Word operations (32-bit, must be 4-byte aligned)
    uint32_t readWord(uint32_t addr) const;
    void writeWord(uint32_t addr, uint32_t val);
    
    // Utility functions
    void reset();
    uint32_t getSize() const { return MEMORY_SIZE; }
    
    // Exception classes for RV32I compliance
    class MemoryException : public std::exception {
    protected:
        std::string message;
    public:
        MemoryException(const std::string& msg) : message(msg) {}
        const char* what() const noexcept override { return message.c_str(); }
    };
    
    class AddressMisalignedException : public MemoryException {
    public:
        AddressMisalignedException(uint32_t addr, uint32_t alignment) 
            : MemoryException("Address 0x" + std::to_string(addr) + 
                            " is not aligned to " + std::to_string(alignment) + " bytes") {}
    };
    
    class AddressOutOfBoundsException : public MemoryException {
    public:
        AddressOutOfBoundsException(uint32_t addr) 
            : MemoryException("Address 0x" + std::to_string(addr) + " is out of bounds") {}
    };
};

#endif // MEMORY_H
