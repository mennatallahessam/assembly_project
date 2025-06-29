.data
equal_msg: .asciiz "Equal"
not_equal_msg: .asciiz "Not Equal"

.text
    li a0, 10
    li a1, 10

    beq a0, a1, label_equal

    la a1, not_equal_msg
    ecall 3
    j end

label_equal:
    la a1, equal_msg
    ecall 3

end:
    ecall 10
