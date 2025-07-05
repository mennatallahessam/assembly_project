.data
.text
    li a0, 0        

    jal label1      
    
 
    li a0, 9999     
    ecall 3        

label1:
    li a0, 10       
    ecall 3         

    j end_label     


    li a0, 8888
    ecall 3        

end_label:
    li a0, 55
    ecall 3        

    ecall 10        
