.data
less_msg: .asciiz "Less than"
not_less_msg: .asciiz "Not less than"

.text
    li a0, 5
    li a1, 10

    blt a0, a1, label_less

    la a1, not_less_msg
    ecall 3
    j end

label_less:
    la a1, less_msg
    ecall 3

end:
    ecall 10
