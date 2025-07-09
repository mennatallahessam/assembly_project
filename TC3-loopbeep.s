.data
msg: .string "A"
.text
    li t0, 3              
loop:
    li16 a0, 400         
    li16 a1, 200          
    ecall 4               
    addi t0, -1           
    bne t0, x0, loop      
    li16 a0, msg          
    ecall 3               
    ecall 10             
