.data
    var1: .word 0       
    var2: .word 0       
    var3: .byte 0      
.text
    li a0, 1234         
    li a1, -1           

 
    la t0, var1
    mv t1, a0
    sw t1, 0(t0)


    la t2, var2
    sw a1, 0(t2)


    lw s0, 0(t0)
    mv a0, s0
    ecall 3             


    lb s1, 0(t2)
    mv a0, s1
    ecall 3             


    lbu s2, 0(t2)
    mv a0, s2
    ecall 3             

    ecall 10           
