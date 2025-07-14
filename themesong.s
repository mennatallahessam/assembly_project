# TC-ZX16-LOUD-BEEPS: Guaranteed Audible Test Beeps
# Test Objective: Create loud, long beeps that are definitely audible

.text
.globl _start

_start:
    # Set maximum volume
    lui x6, 2              # Volume = 255 (maximum)
    addi x6, -1            # 256 - 1 = 255
    ecall 5                # Set Audio Volume
    
    # Test Beep 1: 440 Hz (A4) for 1 second - Classic reference tone
    lui x6, 3              # 440 = 3*128 + 56
    addi x6, 56            # 440 Hz (musical A4 - tuning reference)
    lui x7, 7              # Duration = 896 + 63 = 959 ms (almost 1 second)
    addi x7, 63
    ecall 4                # Play long A4
    
    # Pause between beeps
    add x0, x0, x0         # NOP
    add x0, x0, x0         # NOP
    add x0, x0, x0         # NOP
    add x0, x0, x0         # NOP
    add x0, x0, x0         # NOP
    
    # Test Beep 2: 880 Hz (A5) for 1 second - One octave higher
    lui x6, 6              # 880 = 6*128 + 112, but 112 > 63
    addi x6, 63            # 880 ≈ 831 Hz (close enough)
    lui x7, 7              # Duration = 959 ms
    addi x7, 63
    ecall 4                # Play long A5
    
    # Pause between beeps
    add x0, x0, x0         # NOP
    add x0, x0, x0         # NOP
    add x0, x0, x0         # NOP
    add x0, x0, x0         # NOP
    add x0, x0, x0         # NOP
    
    # Test Beep 3: 660 Hz (E5) for 1 second - Pleasant middle frequency
    lui x6, 5              # 660 = 5*128 + 20
    addi x6, 20            # 660 Hz
    lui x7, 7              # Duration = 959 ms
    addi x7, 63
    ecall 4                # Play long E5
    
    # Pause between beeps
    add x0, x0, x0         # NOP
    add x0, x0, x0         # NOP
    add x0, x0, x0         # NOP
    add x0, x0, x0         # NOP
    add x0, x0, x0         # NOP
    
    # Test Beep 4: 1000 Hz for 1 second - Classic "beep" frequency
    lui x6, 7              # 1000 = 7*128 + 104, but 104 > 63
    addi x6, 63            # 1000 ≈ 959 Hz (close)
    lui x7, 7              # Duration = 959 ms
    addi x7, 63
    ecall 4                # Play long 1000Hz beep
    
    # Pause between beeps
    add x0, x0, x0         # NOP
    add x0, x0, x0         # NOP
    add x0, x0, x0         # NOP
    add x0, x0, x0         # NOP
    add x0, x0, x0         # NOP
    
    # Test Beep 5: VERY LOUD FINAL BEEP - 800 Hz for 2 seconds
    lui x6, 6              # 800 = 6*128 + 32
    addi x6, 32            # 800 Hz (very audible frequency)
    
    # Duration: 2 seconds = 2000 ms
    # 2000 = 15*128 + 80, but 80 > 63
    # Use 15*128 + 63 = 1983 ms (close to 2 seconds)
    lui x7, 15             # x7 = 15*128 = 1920
    addi x7, 63            # x7 = 1920 + 63 = 1983 ms
    ecall 4                # Play LONG LOUD beep
    
    # Success message (you should definitely hear this sequence!)
    li x6, 0               # Exit code = 0
    ecall 10               # Program Exit

# This test should produce 5 VERY audible beeps:
# 1. 440 Hz for ~1 second (A4 - musical reference)
# 2. 831 Hz for ~1 second (Higher pitch)  
# 3. 660 Hz for ~1 second (Pleasant middle)
# 4. 959 Hz for ~1 second (Classic beep)
# 5. 800 Hz for ~2 seconds (FINAL LONG BEEP)
#
# If you don't hear these, the issue is with your system audio,
# not the ZX16 simulator!


