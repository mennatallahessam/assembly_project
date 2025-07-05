.data
.text
    li a0, 10     
    li a1, 20      
    
    mv t0, a0
    add t0, a1     
    
    mv t1,a0
    sub t1, a0     
    
    mv s0,a0
    and s0, a1     
    
    mv  s1,a0
    or  s1, a1     
    
    mv sp, a0
    xor sp, a1    

    
    xor a0, a0
    add a0, t0
    ecall 3         

  
    xor a0, a0
    add a0, t1
    ecall 3       

 
    xor a0, a0
    add a0, s0
    ecall 3         

   
    xor a0, a0
    add a0, s1
    ecall 3         


    xor a0, a0
    add a0, sp
    ecall 3         

    ecall 10         