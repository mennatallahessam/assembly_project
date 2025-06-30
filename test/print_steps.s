.text
.org 0x000
    j main

.org 0x0020
main:
    # r1 = 10
    addi r1, 10

    # print r1
    mov r11, r1      # r11 = r1 (for printInt)
    addi r10, 1      # syscall 1 (printInt)
    ecall

    # r2 = 20
    addi r2, 20

    # print r2
    mov r11, r2
    addi r10, 1
    ecall

    # r3 = r1 + r2
    add r3, r1, r2

    # print r3
    mov r11, r3
    addi r10, 1
    ecall

    # exit
    addi r10, 10
    ecall
