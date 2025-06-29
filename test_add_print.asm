.data
msg: .asciiz "Addition complete."

.text
    li a0, 5
    li a1, 7
    add t0, a0     # t0 = 12

    xor a0, a0
    add a0, t0
    ecall 1        # print 12

    la a1, msg
    ecall 3        # print message

    ecall 10       # exit
