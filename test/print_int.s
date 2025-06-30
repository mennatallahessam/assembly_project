.text
.org 0x000
    j main

.org 0x0020
main:
    addi r11, 42       # load 42 into r11
    addi r10, 1        # syscall 1: print integer in r11
    ecall

    addi r10, 10       # syscall 10: exit
    ecall
