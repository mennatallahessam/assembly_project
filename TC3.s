.data
.text
    li a0, 5        

    addi t0, 3      
    slti t1, 10     
    sltui s0, 4     
    slli s1, 2      
    srli sp, 1      
    srai a1, 1      
    ori t2, 6       
    andi t3, 1      
    xori s2, 3      
    li  s3, 42     

 
    mv a0, t0
    ecall 3

    mv a0, t1
    ecall 3

    mv a0, s0
    ecall 3

    mv a0, s1
    ecall 3

    mv a0, sp
    ecall 3

    mv a0, a1
    ecall 3

    mv a0, t2
    ecall 3

    mv a0, t3
    ecall 3

    mv a0, s2
    ecall 3

    mv a0, s3
    ecall 3

    ecall 10
