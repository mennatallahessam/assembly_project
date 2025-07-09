    .data
    msg: .string "A" 
    .text
    li16 a0, 523          
    li16 a1, 400          
    ecall 4   
    li16 a0, msg           
    ecall 3           
    ecall 10           
