# TC-ZX16-AUDIO-FIXED: ZX16 Audio System Test Case (Correct ZX16 ISA Syntax)
# Test Objective: Validate all audio-related ECALL services using proper ZX16 syntax

.text
.globl _start

_start:
    # ZX16 Audio System Test - Following official ISA specification
    
    # Test 1: Set initial volume using proper immediate construction
    # Need volume = 128, using LUI: 128 = 1 << 7
    lui x6, 1              # x6 = 1 << 7 = 128
    ecall 5                # Set Audio Volume to 128
    
    # Test 2: Play low frequency tone
    # Frequency = 440 Hz. Since imm7 range is -64 to +63, use LUI + ADDI
    # 440 = 384 + 56 = (3 << 7) + 56
    lui x6, 3              # x6 = 3 << 7 = 384  
    addi x6, 56            # x6 = 384 + 56 = 440
    
    # Duration = 1000 ms. 1000 = 896 + 104, but 104 > 63
    # Try: 1000 = 7*128 + 104 = 7*128 + 64 + 40
    lui x7, 7              # x7 = 7 << 7 = 896
    addi x7, 63            # x7 = 896 + 63 = 959 (close to 1000)
    ecall 4                # Play Tone (440 Hz, ~1000 ms)
    
    # Test 3: Set maximum volume (255)
    # 255 = 2*128 - 1 = (2 << 7) - 1
    lui x6, 2              # x6 = 2 << 7 = 256
    addi x6, -1            # x6 = 256 - 1 = 255
    ecall 5                # Set Audio Volume to 255
    
    # Test 4: Play higher frequency tone
    # 880 Hz = 7*128 - 16 = (7 << 7) - 16
    lui x6, 7              # x6 = 7 << 7 = 896
    addi x6, -16           # x6 = 896 - 16 = 880
    
    # Duration = 500 ms = 4*128 - 12 = (4 << 7) - 12
    lui x7, 4              # x7 = 4 << 7 = 512
    addi x7, -12           # x7 = 512 - 12 = 500
    ecall 4                # Play Tone (880 Hz, 500 ms)
    
    # Test 5: Play sequence of beeps with different frequencies
    # Beep 1: 1000 Hz = 7*128 + 104, approximate with 7*128 + 63 = 959
    lui x6, 7              # 896
    addi x6, 63            # 959 Hz (close to 1000)
    
    # Duration 200 ms = 128 + 72, but 72 > 63, so use 128 + 63 = 191
    lui x7, 1              # 128  
    addi x7, 63            # 191 ms (close to 200)
    ecall 4                # Play Tone
    
    # Brief pause using NOPs
    add x0, x0, x0         # NOP
    add x0, x0, x0         # NOP
    add x0, x0, x0         # NOP
    
    # Beep 2: 1200 Hz = 9*128 + 48 = (9 << 7) + 48
    lui x6, 9              # x6 = 9 << 7 = 1152
    addi x6, 48            # x6 = 1152 + 48 = 1200
    lui x7, 1              # Duration ~200ms
    addi x7, 63            # 191 ms
    ecall 4                # Play Tone
    
    # Brief pause
    add x0, x0, x0         # NOP
    add x0, x0, x0         # NOP
    add x0, x0, x0         # NOP
    
    # Beep 3: 1400 Hz = 10*128 + 120, but 120 > 63
    # Use 10*128 + 63 = 1343 (close to 1400)
    lui x6, 10             # x6 = 10 << 7 = 1280
    addi x6, 63            # x6 = 1280 + 63 = 1343
    lui x7, 1              # Duration ~200ms  
    addi x7, 63            # 191 ms
    ecall 4                # Play Tone
    
    # Test 6: Set volume to minimum (mute)
    li x6, 0               # Volume = 0 (mute) - fits in imm7 range
    ecall 5                # Set Audio Volume
    
    # Test 7: Play tone while muted (should be silent)
    lui x6, 3              # 440 Hz frequency
    addi x6, 56
    lui x7, 7              # ~1000 ms duration
    addi x7, 63
    ecall 4                # Play Tone (should be silent)
    
    # Test 8: Test Stop Audio functionality  
    # First restore volume
    lui x6, 1              # Volume = 128
    ecall 5                # Set Audio Volume
    
    # Start a long tone
    lui x6, 3              # 440 Hz
    addi x6, 56
    
    # Duration = 5000 ms = 39*128 + 8 = (39 << 7) + 8
    lui x7, 39             # x7 = 39 << 7 = 4992
    addi x7, 8             # x7 = 4992 + 8 = 5000
    ecall 4                # Play Tone (5 second tone)
    
    # Immediately stop it
    ecall 6                # Stop Audio
    
    # Test 9: Error condition testing
    # Test invalid frequency (0)
    li x6, 0               # Invalid frequency = 0
    lui x7, 7              # Valid duration
    addi x7, 63
    ecall 4                # Should return error
    
    # Test invalid duration (0)  
    lui x6, 3              # Valid frequency
    addi x6, 56
    li x7, 0               # Invalid duration = 0
    ecall 4                # Should return error
    
    # Test invalid volume (> 255), though this may be clamped
    lui x6, 2              # 256 + something > 255
    addi x6, 44            # x6 = 256 + 44 = 300
    ecall 5                # May fail or be clamped to 255
    
    # Test 10: Final audio test - success melody
    # Reset to good volume first
    lui x6, 1              # Volume = 200 = 128 + 72, but 72 > 63
    addi x6, 63            # Volume = 128 + 63 = 191 (close to 200)
    ecall 5                # Set Audio Volume
    
    # Play rising tone sequence (C-E-G major chord)
    # C5 = 523 Hz = 4*128 + 11 = (4 << 7) + 11  
    lui x6, 4              # 512
    addi x6, 11            # 523 Hz
    lui x7, 2              # Duration = 300 ms = 2*128 + 44
    addi x7, 44            # 300 ms
    ecall 4                # Play C5
    
    # E5 = 659 Hz = 5*128 + 19 = (5 << 7) + 19
    lui x6, 5              # 640  
    addi x6, 19            # 659 Hz
    lui x7, 2              # Duration = 300 ms
    addi x7, 44            
    ecall 4                # Play E5
    
    # G5 = 784 Hz = 6*128 + 16 = (6 << 7) + 16
    lui x6, 6              # 768
    addi x6, 16            # 784 Hz  
    lui x7, 3              # Duration = 500 ms = 3*128 + 116, but 116 > 63
    addi x7, 63            # Duration = 3*128 + 63 = 447 ms (close to 500)
    ecall 4                # Play G5
    
    # Test completion - exit program
    li x6, 0               # Exit code = 0 (fits in imm7 range)
    ecall 10               # Program Exit



