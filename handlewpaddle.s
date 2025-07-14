# Key code constants
.equ KEY_W, 87      # 'W' key
.equ KEY_S, 83      # 'S' key

multiply_by_20:
    ADD x5, x4, x0        # Save original value in x5
    SLLI x4, x4, 4        # x4 = x4 * 16 (shift left by 4)
    SLLI x5, x5, 2        # x5 = original * 4 (shift left by 2)
    ADD x4, x4, x5        # x4 = 16*original + 4*original = 20*original
    JALR x0, x0, 0        # Return (jump back to caller)

main:
    # === Setup Color Palette ===
    LUI x3, 500       # 0xFA00
    LI x4, 0          # Black
    SB x4, 0(x3)      # palette[0] = black
    LI x4, -1         # White (0xFF)
    SB x4, 1(x3)      # palette[1] = white
    
    # === Create White Block Tile (Tile 1) ===
    LUI x3, 484       # 0xF200
    ADDI x3, 63       # Skip to tile 1
    ADDI x3, 63
    ADDI x3, 2        # Now at 0xF280 (tile 1)
    
    # Fill tile 1 with white pixels
    LI x4, 17         # 0x11 = white pixels
    
    # Fill enough bytes to make tile visible
    SB x4, 0(x3)
    SB x4, 1(x3)
    SB x4, 2(x3)
    SB x4, 3(x3)
    SB x4, 4(x3)
    SB x4, 5(x3)
    SB x4, 6(x3)
    SB x4, 7(x3)
    
    ADDI x3, 8
    SB x4, 0(x3)
    SB x4, 1(x3)
    SB x4, 2(x3)
    SB x4, 3(x3)
    SB x4, 4(x3)
    SB x4, 5(x3)
    SB x4, 6(x3)
    SB x4, 7(x3)
    
    ADDI x3, 8
    SB x4, 0(x3)
    SB x4, 1(x3)
    SB x4, 2(x3)
    SB x4, 3(x3)
    SB x4, 4(x3)
    SB x4, 5(x3)
    SB x4, 6(x3)
    SB x4, 7(x3)
    
    ADDI x3, 8
    SB x4, 0(x3)
    SB x4, 1(x3)
    SB x4, 2(x3)
    SB x4, 3(x3)
    SB x4, 4(x3)
    SB x4, 5(x3)
    SB x4, 6(x3)
    SB x4, 7(x3)

    ADDI x3, 8
    SB x4, 0(x3)
    SB x4, 1(x3)
    SB x4, 2(x3)
    SB x4, 3(x3)
    SB x4, 4(x3)
    SB x4, 5(x3)
    SB x4, 6(x3)
    SB x4, 7(x3)

    ADDI x3, 8
    SB x4, 0(x3)
    SB x4, 1(x3)
    SB x4, 2(x3)
    SB x4, 3(x3)
    SB x4, 4(x3)
    SB x4, 5(x3)
    SB x4, 6(x3)
    SB x4, 7(x3)

    ADDI x3, 8
    SB x4, 0(x3)
    SB x4, 1(x3)
    SB x4, 2(x3)
    SB x4, 3(x3)
    SB x4, 4(x3)
    SB x4, 5(x3)
    SB x4, 6(x3)
    SB x4, 7(x3)

    ADDI x3, 8
    SB x4, 0(x3)
    SB x4, 1(x3)
    SB x4, 2(x3)
    SB x4, 3(x3)
    SB x4, 4(x3)
    SB x4, 5(x3)
    SB x4, 6(x3)
    SB x4, 7(x3)

    ADDI x3, 8
    SB x4, 0(x3)
    SB x4, 1(x3)
    SB x4, 2(x3)
    SB x4, 3(x3)
    SB x4, 4(x3)
    SB x4, 5(x3)
    SB x4, 6(x3)
    SB x4, 7(x3)

    ADDI x3, 8
    SB x4, 0(x3)
    SB x4, 1(x3)
    SB x4, 2(x3)
    SB x4, 3(x3)
    SB x4, 4(x3)
    SB x4, 5(x3)
    SB x4, 6(x3)
    SB x4, 7(x3)

    ADDI x3, 8
    SB x4, 0(x3)
    SB x4, 1(x3)
    SB x4, 2(x3)
    SB x4, 3(x3)
    SB x4, 4(x3)
    SB x4, 5(x3)
    SB x4, 6(x3)
    SB x4, 7(x3)

    ADDI x3, 8
    SB x4, 0(x3)
    SB x4, 1(x3)
    SB x4, 2(x3)
    SB x4, 3(x3)
    SB x4, 4(x3)
    SB x4, 5(x3)
    SB x4, 6(x3)
    SB x4, 7(x3)

    ADDI x3, 8
    SB x4, 0(x3)
    SB x4, 1(x3)
    SB x4, 2(x3)
    SB x4, 3(x3)
    SB x4, 4(x3)
    SB x4, 5(x3)
    SB x4, 6(x3)
    SB x4, 7(x3)

    ADDI x3, 8
    SB x4, 0(x3)
    SB x4, 1(x3)
    SB x4, 2(x3)
    SB x4, 3(x3)
    SB x4, 4(x3)
    SB x4, 5(x3)
    SB x4, 6(x3)
    SB x4, 7(x3)

    ADDI x3, 8
    SB x4, 0(x3)
    SB x4, 1(x3)
    SB x4, 2(x3)
    SB x4, 3(x3)
    SB x4, 4(x3)
    SB x4, 5(x3)
    SB x4, 6(x3)
    SB x4, 7(x3)

show_pong_text:
    # === Clear screen and place white tiles directly ===
    LUI x3, 480       # 0xF000 (tile map)
    LI x4, 1          # White tile (tile 1)
    
    # Create visible pattern by placing white tiles
    # First, clear some area
    LI x5, 0          # Black tile
    SB x5, 0(x3)
    SB x5, 1(x3)
    SB x5, 2(x3)
    SB x5, 3(x3)
    SB x5, 4(x3)
    SB x5, 5(x3)
    SB x5, 6(x3)
    SB x5, 7(x3)
    
    # Move to next row
    ADDI x3, 20
    SB x5, 0(x3)
    SB x5, 1(x3)
    SB x5, 2(x3)
    SB x5, 3(x3)
    SB x5, 4(x3)
    SB x5, 5(x3)
    SB x5, 6(x3)
    SB x5, 7(x3)
    
    # === Create BIG, SOLID "PONG GAME" text pattern ===
    
    # Letter "P" - Position: Row 2, Col 1
    LUI x3, 480     # Reset to tile map
    ADDI x3, 40       # Row 2: 2*20 = 40
    ADDI x3, 1        # Column 1
    
    # P - make it 3x4 blocks for visibility
    # Row 1: ███
    SB x4, 0(x3)      # █
    SB x4, 1(x3)      # █
    SB x4, 2(x3)      # █
    # Row 2: █ █
    ADDI x3, 20
    SB x4, 0(x3)      # █
    SB x4, 2(x3)      # █
    # Row 3: ███
    ADDI x3, 20
    SB x4, 0(x3)      # █
    SB x4, 1(x3)      # █
    SB x4, 2(x3)      # █
    # Row 4: █
    ADDI x3, 20
    SB x4, 0(x3)      # █
    # Row 5: █
    ADDI x3, 20
    SB x4, 0(x3)      # █
    
    # Letter "O" - Position: Row 2, Col 5
    LUI x3, 480       # Reset
    ADDI x3, 40       # Row 2
    ADDI x3, 5        # Column 5
    
    # O - make it 3x4 blocks
    # Row 1: ███
    SB x4, 0(x3)      # █
    SB x4, 1(x3)      # █
    SB x4, 2(x3)      # █
    # Row 2: █ █
    ADDI x3, 20
    SB x4, 0(x3)      # █
    SB x4, 2(x3)      # █
    # Row 3: █ █
    ADDI x3, 20
    SB x4, 0(x3)      # █
    SB x4, 2(x3)      # █
    # Row 4: █ █
    ADDI x3, 20
    SB x4, 0(x3)      # █
    SB x4, 2(x3)      # █
    # Row 5: ███
    ADDI x3, 20
    SB x4, 0(x3)      # █
    SB x4, 1(x3)      # █
    SB x4, 2(x3)      # █
    
    # Letter "N" - Position: Row 2, Col 9
    LUI x3, 480       # Reset
    ADDI x3, 40       # Row 2
    ADDI x3, 9        # Column 9
    
    # N - make it 3x4 blocks
    # Row 1: █ █
    SB x4, 0(x3)      # █
    SB x4, 3(x3)      # █
    # Row 2: ███
    ADDI x3, 20
    SB x4, 0(x3)      # █
    SB x4, 1(x3)      # █
    SB x4, 3(x3)      # █

    # Row 3: █ █
    ADDI x3, 20
    SB x4, 0(x3)      # █
    SB x4, 2(x3) 
    SB x4, 3(x3) 

    # Row 4: █ █
    ADDI x3, 20
    SB x4, 0(x3)      # █
    SB x4, 3(x3)      # █
    # Row 5: █ █
    ADDI x3, 20
    SB x4, 0(x3)      # █
    SB x4, 3(x3)      # █

    # Letter "G" - Position: Row 2, Col 13
    LUI x3, 480       # Reset
    ADDI x3, 40       # Row 2
    ADDI x3, 14      # Column 13
    
    # G - make it 3x4 blocks
    # Row 1: ███
    SB x4, 0(x3)      # █
    SB x4, 1(x3)      # █
    SB x4, 2(x3)      # █
    # Row 2: █
    ADDI x3, 20
    SB x4, 0(x3)      # █
    # Row 3: █ ██
    ADDI x3, 20
    SB x4, 0(x3)      # █
    SB x4, 1(x3)      # █
    SB x4, 2(x3)      # █
    # Row 4: █  █
    ADDI x3, 20
    SB x4, 0(x3)      # █
    SB x4, 2(x3)      # █
    # Row 5: ███
    ADDI x3, 20
    SB x4, 0(x3)      # █
    SB x4, 1(x3)      # █
    SB x4, 2(x3)      # █

    # === "GAME" text below ===
    
    # Letter "G" for GAME - Position: Row 8, Col 1
    LUI x3, 480       # Reset
    ADDI x3, 63       # 63
    ADDI x3, 63       # 126
    ADDI x3, 35       # 161 (row 8)
    ADDI x3, 0        # Column 1
    
    # G - make it 3x4 blocks
    # Row 1: ███
    SB x4, 0(x3)      # █
    SB x4, 1(x3)      # █
    SB x4, 2(x3)      # █
    # Row 2: █
    ADDI x3, 20
    SB x4, 0(x3)      # █
    # Row 3: █ ██
    ADDI x3, 20
    SB x4, 0(x3)      # █
    SB x4, 1(x3)      # █
    SB x4, 2(x3)      # █
    # Row 4: █  █
    ADDI x3, 20
    SB x4, 0(x3)      # █
    SB x4, 2(x3)      # █
    # Row 5: ███
    ADDI x3, 20
    SB x4, 0(x3)      # █
    SB x4, 1(x3)      # █
    SB x4, 2(x3)      # █

    # Letter "A" - Position: Row 8, Col 5
    LUI x3, 480       # Reset
    ADDI x3, 63       
    ADDI x3, 63       
    ADDI x3, 34       
    ADDI x3, 5        # Column 5
    
    # A - 3x5 blocks
    SB x4, 0(x3)      # █
    SB x4, 1(x3)      # █
    SB x4, 2(x3)      # █
    ADDI x3, 20
    SB x4, 0(x3)      # █
    SB x4, 2(x3)      # █
    ADDI x3, 20
    SB x4, 0(x3)      # █
    SB x4, 1(x3)      # █
    SB x4, 2(x3)      # █
    ADDI x3, 20
    SB x4, 0(x3)      # █
    SB x4, 2(x3)      # █
    ADDI x3, 20
    SB x4, 0(x3)      # █
    SB x4, 2(x3)      # █

    # Letter "M" - Position: Row 8, Col 9
    LUI x3, 480       # Reset
    ADDI x3, 63       
    ADDI x3, 63       
    ADDI x3, 34       
    ADDI x3, 9        # Column 9
    
    # M - 3x5 blocks
    SB x4, 0(x3)      # █
    SB x4, 1(x3)      # █
    SB x4, 2(x3)      # █
    ADDI x3, 20
    SB x4, 0(x3)      # █
    SB x4, 1(x3)      # █
    SB x4, 2(x3)      # █
    ADDI x3, 20
    SB x4, 0(x3)      # █
    SB x4, 1(x3)      # █
    SB x4, 2(x3)      # █
    ADDI x3, 20
    SB x4, 0(x3)      # █
    SB x4, 2(x3)      # █
    ADDI x3, 20
    SB x4, 0(x3)      # █
    SB x4, 2(x3)      # █
    
    # Letter "E" - Position: Row 8, Col 13
    LUI x3, 480       # Reset
    ADDI x3, 63       
    ADDI x3, 63       
    ADDI x3, 34       
    ADDI x3, 13       # Column 13
    
    # E - 3x5 blocks
    SB x4, 0(x3)      # █
    SB x4, 1(x3)      # █
    SB x4, 2(x3)      # █
    ADDI x3, 20
    SB x4, 0(x3)      # █
    ADDI x3, 20
    SB x4, 0(x3)      # █
    SB x4, 1(x3)      # █
    SB x4, 2(x3)  
    ADDI x3, 20
    SB x4, 0(x3)      # █
    ADDI x3, 20
    SB x4, 0(x3)      # █
    SB x4, 1(x3)      # █
    SB x4, 2(x3)      # █

clear_pong_text:
    LUI x3, 480       # Reset to tile map
    LI x4, 0          # Black tile
    
    ADDI x3, 40       # Row 2: 2*20 = 40
    ADDI x3, 1        # Column 1
    
    # Clear P
    SB x4, 0(x3)      # █
    SB x4, 1(x3)      # █
    SB x4, 2(x3)      # █
    ADDI x3, 20
    SB x4, 0(x3)      # █
    SB x4, 2(x3)      # █
    ADDI x3, 20
    SB x4, 0(x3)      # █
    SB x4, 1(x3)      # █
    SB x4, 2(x3)      # █
    ADDI x3, 20
    SB x4, 0(x3)      # █
    ADDI x3, 20
    SB x4, 0(x3)      # █
    
    # Clear O
    LUI x3, 480       # Reset
    ADDI x3, 40       # Row 2
    ADDI x3, 5        # Column 5
    
    SB x4, 0(x3)      # █
    SB x4, 1(x3)      # █
    SB x4, 2(x3)      # █
    ADDI x3, 20
    SB x4, 0(x3)      # █
    SB x4, 2(x3)      # █
    ADDI x3, 20
    SB x4, 0(x3)      # █
    SB x4, 2(x3)      # █
    ADDI x3, 20
    SB x4, 0(x3)      # █
    SB x4, 2(x3)      # █
    ADDI x3, 20
    SB x4, 0(x3)      # █
    SB x4, 1(x3)      # █
    SB x4, 2(x3)      # █
    
    # Clear N
    LUI x3, 480       # Reset
    ADDI x3, 40       # Row 2
    ADDI x3, 9        # Column 9
    
    SB x4, 0(x3)      # █
    SB x4, 3(x3)      # █
    ADDI x3, 20
    SB x4, 0(x3)      # █
    SB x4, 1(x3)      # █
    SB x4, 3(x3)      # █
    ADDI x3, 20
    SB x4, 0(x3)      # █
    SB x4, 2(x3) 
    SB x4, 3(x3) 
    ADDI x3, 20
    SB x4, 0(x3)      # █
    SB x4, 3(x3)      # █
    ADDI x3, 20
    SB x4, 0(x3)      # █
    SB x4, 3(x3)      # █

    # Clear G
    LUI x3, 480       # Reset
    ADDI x3, 40       # Row 2
    ADDI x3, 14      # Column 14
    
    SB x4, 0(x3)      # █
    SB x4, 1(x3)      # █
    SB x4, 2(x3)      # █
    ADDI x3, 20
    SB x4, 0(x3)      # █
    ADDI x3, 20
    SB x4, 0(x3)      # █
    SB x4, 1(x3)      # █
    SB x4, 2(x3)      # █
    ADDI x3, 20
    SB x4, 0(x3)      # █
    SB x4, 2(x3)      # █
    ADDI x3, 20
    SB x4, 0(x3)      # █
    SB x4, 1(x3)      # █
    SB x4, 2(x3)      # █

    # Clear "GAME" text
    LUI x3, 480       # Reset
    ADDI x3, 63       
    ADDI x3, 63       
    ADDI x3, 35       
    ADDI x3, 0        
    
    SB x4, 0(x3)      # █
    SB x4, 1(x3)      # █
    SB x4, 2(x3)      # █
    ADDI x3, 20
    SB x4, 0(x3)      # █
    ADDI x3, 20
    SB x4, 0(x3)      # █
    SB x4, 1(x3)      # █
    SB x4, 2(x3)      # █
    ADDI x3, 20
    SB x4, 0(x3)      # █
    SB x4, 2(x3)      # █
    ADDI x3, 20
    SB x4, 0(x3)      # █
    SB x4, 1(x3)      # █
    SB x4, 2(x3)      # █

    # Clear A
    LUI x3, 480       # Reset
    ADDI x3, 63       
    ADDI x3, 63       
    ADDI x3, 34       
    ADDI x3, 5        
    
    SB x4, 0(x3)      # █
    SB x4, 1(x3)      # █
    SB x4, 2(x3)      # █
    ADDI x3, 20
    SB x4, 0(x3)      # █
    SB x4, 2(x3)      # █
    ADDI x3, 20
    SB x4, 0(x3)      # █
    SB x4, 1(x3)      # █
    SB x4, 2(x3)      # █
    ADDI x3, 20
    SB x4, 0(x3)      # █
    SB x4, 2(x3)      # █
    ADDI x3, 20
    SB x4, 0(x3)      # █
    SB x4, 2(x3)      # █

    # Clear M
    LUI x3, 480       # Reset
    ADDI x3, 63       # 63
    ADDI x3, 63       # 126
    ADDI x3, 34       # 160 (row 8)
    ADDI x3, 9        # Column 9
    
    # M - 3x4 blocks
    SB x4, 0(x3)      # █
    SB x4, 1(x3)      # █
    SB x4, 2(x3)      # █
    ADDI x3, 20
    SB x4, 0(x3)      # █
    SB x4, 1(x3)      # █
    SB x4, 2(x3)      # █
    ADDI x3, 20
    SB x4, 0(x3)      # █
    SB x4, 1(x3)      # █
    SB x4, 2(x3)      # █
    ADDI x3, 20
    SB x4, 0(x3)      # █
    SB x4, 2(x3)      # █
    ADDI x3, 20
    SB x4, 0(x3)      # █
    SB x4, 2(x3)      # █
    
    # Clear E
    LUI x3, 480       # Reset
    ADDI x3, 63       # 63
    ADDI x3, 63       # 126
    ADDI x3, 34       # 160 (row 8)
    ADDI x3, 13       # Column 13
    
    # E - 3x4 blocks
    SB x4, 0(x3)      # █
    SB x4, 1(x3)      # █
    SB x4, 2(x3)      # █
    ADDI x3, 20
    SB x4, 0(x3)      # █
    ADDI x3, 20
    SB x4, 0(x3)      # █
    SB x4, 1(x3)      # █
    SB x4, 2(x3)  
    ADDI x3, 20
    SB x4, 0(x3)      # █
    ADDI x3, 20
    SB x4, 0(x3)      # █
    SB x4, 1(x3)      # █
    SB x4, 2(x3)      # █

    # === NOW SHOW GAME SCREEN WITH CORRECT POSITIONS ===
    LI x4, 1          # White tile
    
    # === LEFT PADDLE - Column 0 (absolute leftmost), bigger size ===
    LUI x3, 480       # Reset to tile map base (0xF000)
    ADDI x3, 60       # Row 3: 3*20 = 60 (start higher for bigger paddle)
    ADDI x3, 0        # Column 0 (absolute leftmost edge)
    
    # Draw bigger left paddle (4 tiles tall)
    SB x4, 0(x3)      # Row 3, Column 0
    ADDI x3, 20       # Next row
    SB x4, 0(x3)      # Row 4, Column 0
    ADDI x3, 20       # Next row
    SB x4, 0(x3)      # Row 5, Column 0
    ADDI x3, 20       # Next row
    SB x4, 0(x3)      # Row 6, Column 0
      
    # === RIGHT PADDLE - Column 19 (absolute rightmost), bigger size ===
    LUI x3, 480       # Reset to tile map base (0xF000)
    ADDI x3, 60       # Row 3: 3*20 = 60 (start higher for bigger paddle)
    ADDI x3, 19      # Column 19 (absolute rightmost edge)
    
    # Draw bigger right paddle (4 tiles tall)
    SB x4, 0(x3)      # Row 3, Column 19
    ADDI x3, 20       # Next row
    SB x4, 0(x3)      # Row 4, Column 19
    ADDI x3, 20       # Next row
    SB x4, 0(x3)      # Row 5, Column 19
    ADDI x3, 20       # Next row
    SB x4, 0(x3)      # Row 6, Column 19
       
    # === CENTER BALL - Single tile in the exact center ===
    LUI x3, 480       # Reset to tile map base (0xF000)
    ADDI x3, 63       # Add 63
    ADDI x3, 57       # Add 57 more = 120 total (Row 6: 6*20 = 120)
    ADDI x3, 10       # Column 10 (exact center)
    
    # Draw single ball tile
    SB x4, 0(x3)      # Row 6, Column 10 (center)
        
    # === INITIALIZE PADDLE POSITIONS ===
    LI x1, 3          # Left paddle Y position (row 3)
    LI x2, 3          # Right paddle Y position (row 3)

game_loop:
    ECALL 7                     # Wait for key input
    BEQ x7, x0, game_loop       # If no key pressed, loop again

    # Check for W key (move left paddle up)
    LI x3, KEY_W
    BEQ x6, x3, handle_w_key

    # Check for S key (move left paddle down)
    LI x3, KEY_S
    BEQ x6, x3, handle_s_key

    # If no recognized key, continue loop
    J game_loop

handle_w_key:
    # Check if already at top boundary (row 1)
    LI x3, 1
    BEQ x1, x3, game_loop       # If already at row 1, can't move up

    # Clear old paddle position
    LUI x3, 480                 # Base address 0xF000
    MV x4, x1                   # Current paddle row
    JAL x1, multiply_by_20      # Call multiply function
    ADD x3, x3, x4              # Add offset to base address
    
    # Clear 4 tiles (paddle height)
    LI x4, 0                    # Black tile
    SB x4, 0(x3)                # Clear current row
    ADDI x3, 20
    SB x4, 0(x3)                # Clear next row
    ADDI x3, 20
    SB x4, 0(x3)                # Clear next row
    ADDI x3, 20
    SB x4, 0(x3)                # Clear next row
    
    # Move paddle up (decrease row number)
    ADDI x1, -1
    
    # Draw paddle at new position
    LUI x3, 480                 # Base address 0xF000
    MV x4, x1                   # New paddle row
    JAL x1, multiply_by_20      # Call multiply function
    ADD x3, x3, x4              # Add offset to base address
    
    # Draw 4 tiles (paddle height)
    LI x4, 1                    # White tile
    SB x4, 0(x3)                # Draw current row
    ADDI x3, 20
    SB x4, 0(x3)                # Draw next row
    ADDI x3, 20
    SB x4, 0(x3)                # Draw next row
    ADDI x3, 20
    SB x4, 0(x3)                # Draw next row
    
    J game_loop

handle_s_key:
    # Check if already at bottom boundary (row 13 for 4-tile paddle)
    LI x3, 13
    BEQ x1, x3, game_loop       # If already at row 13, can't move down

    # Clear old paddle position
    LUI x3, 480                 # Base address 0xF000
    MV x4, x1                   # Current paddle row
    JAL x1, multiply_by_20      # Call multiply function
    ADD x3, x3, x4              # Add offset to base address
    
    # Clear 4 tiles (paddle height)
    LI x4, 0                    # Black tile
    SB x4, 0(x3)                # Clear current row
    ADDI x3, 20
    SB x4, 0(x3)                # Clear next row
    ADDI x3, 20
    SB x4, 0(x3)                # Clear next row
    ADDI x3, 20
    SB x4, 0(x3)                # Clear next row
    
    # Move paddle down (increase row number)
    ADDI x1, 1
    
    # Draw paddle at new position
    LUI x3, 480                 # Base address 0xF000
    MV x4, x1                   # New paddle row
    JAL x1, multiply_by_20      # Call multiply function
    ADD x3, x3, x4              # Add offset to base address
    
    # Draw 4 tiles (paddle height)
    LI x4, 1                    # White tile
    SB x4, 0(x3)                # Draw current row
    ADDI x3, 20
    SB x4, 0(x3)                # Draw next row
    ADDI x3, 20
    SB x4, 0(x3)                # Draw next row
    ADDI x3, 20
    SB x4, 0(x3)                # Draw next row
    
    J game_loop

# End program
ECALL 10

