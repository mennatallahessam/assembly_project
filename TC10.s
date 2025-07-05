.data
value: .word 1234
.text
    li a0, 0
    jal linkreg
    li a0, 9999
    ecall 3

linkreg:
    lw a1, value
    ecall 3
    j end

    li a2, 5555
    ecall 3

end:
    ecall 10
