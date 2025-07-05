# ZX16 R-Type Instruction Test Suite (two-operand style)
.text
.globl _start
_start:
    # Test 1: ADD instruction
    addi x1, 15         # x1 = 15
    addi x2, 25         # x2 = 25
    mv   x3, x1         # x3 = x1
    add  x3, x2         # x3 += x2 → 40
    ecall 8             # dump regs
    
    # Test 2: SUB instruction
    addi x1, 50         # x1 = 50
    addi x2, 30         # x2 = 30
    mv   x3, x1         # x3 = 50
    sub  x3, x2         # x3 -= x2 → 20
    ecall 8             # dump regs
    
    # Test 3: SLT true
    addi x1, 10
    addi x2, 20
    mv   x3, x1
    slt  x3, x2         # x3 = (10<20)?1:0 → 1
    ecall 8
    
    # Test 4: SLT false
    addi x1, 30
    addi x2, 10
    mv   x3, x1
    slt  x3, x2         # x3 = (30<10)?1:0 → 0
    ecall 8
    
    # Test 5: SLTU false
    addi x1, -1         # x1=0xFFFF
    addi x2, 1
    mv   x3, x1
    sltu x3, x2         # x3=(0xFFFF<1)?1:0 → 0
    ecall 8
    
    # Test 6: SLTU true
    addi x1, 5
    addi x2, 10
    mv   x3, x1
    sltu x3, x2         # x3=(5<10)?1:0 → 1
    ecall 8
    
    # Test 7: SLL
    addi x1, 5
    addi x2, 2
    mv   x3, x1
    sll  x3, x2         # x3=5<<2 → 20
    ecall 8
    
    # Test 8: SRL
    addi x1, 20
    addi x2, 2
    mv   x3, x1
    srl  x3, x2         # x3=20>>2 → 5
    ecall 8
    
    # Test 9: SRA
    addi x1, -8
    addi x2, 2
    mv   x3, x1
    sra  x3, x2         # x3=–8>>2 → –2
    ecall 8
    
    # Test 10: OR
    addi x1, 12
    addi x2, 10
    mv   x3, x1
    or   x3, x2         # x3=12|10 → 14
    ecall 8
    
    # Test 11: AND
    addi x1, 12
    addi x2, 10
    mv   x3, x1
    and  x3, x2         # x3=12&10 → 8
    ecall 8
    
    # Test 12: XOR
    addi x1, 12
    addi x2, 10
    mv   x3, x1
    xor  x3, x2         # x3=12^10 → 6
    ecall 8
    
    # Test 13: MV
    addi x1, 0
    addi x2, 42
    mv   x3, x2         # x3=42
    ecall 8
    
    # Test 14: JR test — round trip jump (FIXED)
    addi x3, 15             # x3 = 15
    jal x1, jr_setup        # x1 = return address, jumps to jr_setup
    ecall 8                 # this should run after returning from JR
    j after_jr              # skip past jr code

jr_setup:
    addi x4, 10             # x4 = 10 (test value)
    jr x1                   # return to caller (back to ecall 8)

after_jr:
    # Test 15: JALR test
    addi x4, 20         # x4 = 20
    jal x1, jalr_fn     # x1 = return address, jump to fn
    ecall 8             # print x4 = 20
    j end

jalr_fn:
    addi x4, 30         # modify x4 to show function executed
    jr x1               # return

end:
    ecall 10            # halt