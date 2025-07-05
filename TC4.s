.data
.text
    li a0, 5       
    li a1, 5     
    li a2, 7      
    li a3, 3       

  
    beq a0, a1, label1


    li t0, 100     
    ecall 3        

label1:
    li t0, 10
    ecall 3       

   
    bne a0, a2, label2


    li t1, 200
    ecall 3        

label2:
    li t1, 20
    ecall 3        

    
    blt a3, a2, label3
    li t2, 300
    ecall 3        

label3:
    li t2, 30
    ecall 3        

  
    bge a0, a3, label4
    li t3, 400
    ecall 3       

label4:
    li t3, 40
    ecall 3       


    bltu a3, a0, label5
    li s0, 500
    ecall 3        

label5:
    li s0, 50
    ecall 3        

  
    bgeu a2, a3, label6
    li s1, 600
    ecall 3       

label6:
    li s1, 60
    ecall 3        

    ecall 10       
