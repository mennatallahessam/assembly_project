// #ifndef REGISTERS_H
// #define REGISTERS_H
//
// #include <cstdint>
// #include <array>
//
// class Registers {
// private:
//     std::array<uint16_t, 16> regs;
//
// public:
//     Registers();
//
//     // Access register by index (r0 - r15)
//     uint16_t& operator[](int idx);
//
//     // Read-only version of the above (optional, for const use)
//     uint16_t operator[](int idx) const;
//
//     // Dump all register values to stdout
//     void dump() const;
//
//     // Reset all registers to 0 (optional utility)
//     void reset();
// };
//
// #endif // REGISTERS_H
