.data
.text
    li a0, 15      
    li a1, 2       
    li a2, 20      

 
    mv t0, a0
    sll t0, a1     

    mv t1, a0
    srl t1, a1     

    mv t2, a0
    sra t2, a1     


    slt s0, a0     
    sltu s1, a0    


    beq a0, a1, skip_print1  
    li a3, 99
    ecall 3                  

skip_print1:

    bne a0, a1, skip_print2  
    li a4, 77
    ecall 3                 

skip_print2:

 
    xor a0, a0
    add a0, t0       
    ecall 3

    xor a0, a0
    add a0, t1       
    ecall 3

    xor a0, a0
    add a0, t2      
    ecall 3

    xor a0, a0
    add a0, s0       
    ecall 3

    xor a0, a0
    add a0, s1       
    ecall 3

    ecall 10         
