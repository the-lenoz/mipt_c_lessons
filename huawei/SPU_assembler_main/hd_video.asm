
    LEA     F1_ptr  FLAG1

    MOVC    width   256
    MOVC    height  144

    MOVC    ONE     1
    MOVC    TWO     2
    MOVC    FOUR    4
    MOVC    FIVE    5
    
    LEA     current_data_char_ptr   VGA_data
    MUL     R1   width   height  4
    ;ADD     current_data_char_ptr   current_data_char_ptr   R1  4

loopM:
    OUT     0x1     current_data_char_ptr   4


    LEA     R3      current_data_char_ptr

    MUL     R1      width   height  4
    MUL     R1      R1      FOUR    4

    $OUT    TWO      current_data_char_ptr      R1

    ADD current_data_char_ptr current_data_char_ptr R1 4

    MOVC    R3      100
    MOVC    R4      5
    wait_loop:
        IN  0x0 R1  4
        DIV R2  R1  R3  4
        MUL R2  R2  R3  4
        SUB R1  R1  R2  4
        LT  FLAG1   R4  R1  4
        $CLEA   F1_ptr  IP  wait_loop  

    $LEA    IP      loopM
    
;loopM end

    HLT          



; DATA
i:
    NOP
    NOP
    NOP
    NOP

j:
    NOP
    NOP
    NOP
    NOP

vga_i:
    NOP
    NOP
    NOP
    NOP

vga_j:
    NOP
    NOP
    NOP
    NOP



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

R4:
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

current_data_char_ptr:
    NOP
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

TWO:
    NOP
    NOP
    NOP
    NOP

FOUR:
    NOP
    NOP
    NOP
    NOP

FIVE:
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

