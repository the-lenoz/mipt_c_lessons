
    LEA     F1_ptr  FLAG1


    MOVC    width   96
    MOVC    height  48
    
    MOVC    ONE     1

    MOVC N 15000
    MOVC n 0

    MOVC A 0

    MUL R1 width height 4
    
    LEA    current_VGA_char_ptr    VGA_data

loopM:
    ADD current_VGA_char_ptr current_VGA_char_ptr R1 4
    ADD n n ONE 4

    MOVC A 0
    delay:
        ADD A A ONE 4
        LT FLAG1 A N 4
        $CLEA F1_ptr IP delay

    $OUT    ZERO      current_VGA_char_ptr      R1
    


    LT      FLAG1   n     N   4
    $CLEA   F1_ptr  IP      loopM
;loopM end

    HLT          



; DATA
R1:
    NOP
    NOP
    NOP
    NOP

R2:
    NOP
    NOP
    NOP
    NOP

R3:
    NOP
    NOP
    NOP
    NOP


IP:    
    NOP ; zeros
    NOP
    NOP
    NOP

n:
    NOP
    NOP
    NOP
    NOP

N:    
    NOP ; zeros
    NOP
    NOP
    NOP

A: 
    NOP
    NOP
    NOP
    NOP


current_VGA_char_ptr:
    NOP
    NOP
    NOP
    NOP



radius:    
    NOP ; zeros
    NOP
    NOP
    NOP

width:
    NOP ; zeros
    NOP
    NOP
    NOP

height:
    NOP
    NOP
    NOP
    NOP

centerX:
    NOP
    NOP
    NOP
    NOP

centerY:
    NOP
    NOP
    NOP
    NOP


ONE:
    NOP
    NOP
    NOP
    NOP

ZERO:
    NOP
    NOP
    NOP
    NOP

FILLC:
    NOP
    NOP
    NOP
    NOP


EMPTYC:
    NOP
    NOP
    NOP
    NOP


F1_ptr:
    NOP
    NOP
    NOP
    NOP

FLAG1:
    NOP
    NOP
    NOP
    NOP    



VGA_data:

