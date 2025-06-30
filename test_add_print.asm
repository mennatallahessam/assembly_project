.text
.org 0x000
    j main
.org 0x0020
main:
    # x1 = 12
    addi x1, 12
    # x2 = 8
    addi x2, 8
    # ---- Addition ----
    add x6, x1, x2    # x6 = x1 + x2 = 20
    addi r10, 10         # syscall: exit
    ecall


    ecall 10
