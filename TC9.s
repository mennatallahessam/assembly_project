.data
.text
    li a0, 5
    li a1, 5
    li a2, 3
    li a3, 7

    beq a0, a1, equal_branch
    li t0, 111
equal_branch:
    li t0, 100
    ecall 3

    bne a0, a2, notequal_branch
    li t1, 222
notequal_branch:
    li t1, 200
    ecall 3

    blt a2, a3, less_branch
    li t2, 333
less_branch:
    li t2, 300
    ecall 3

    bge a0, a2, ge_branch
    li t3, 444
ge_branch:
    li t3, 400
    ecall 3

    ecall 10
