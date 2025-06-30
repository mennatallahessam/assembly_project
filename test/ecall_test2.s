.text
.org 0x000
    j main

.org 0x0020
main:
    addi r10, 3          # r10 = 3 → syscall: print string
    addi r11, 32         # r11 = 32 → string address
    ecall                # syscall: print string

    addi r10, 10         # syscall: exit
    ecall

    # Padding to make room for string
    .fill 32 - (. - main), 0

    # Null-terminated string at address 32
    .string "Test123"
