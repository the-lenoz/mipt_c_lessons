
    LEA     F1_ptr  FLAG1

    MOVC    width   256
    MOVC    height  144
    
    MOVC    datawidth   256
    MOVC    dataheight  144

    MOVC    ONE     1
    MOVC    TWO     2
    MOVC    FOUR    4
    MOVC    FIVE    5
    
    LEA     current_data_char_ptr   VGA_data
    MUL     R1   width   height  4
    ;ADD     current_data_char_ptr   current_data_char_ptr   R1  4

loopM:
    MOVC    i   0
    LEA     current_VGA_char_ptr    VGA_data
init_fill_loop_y:
    MOVC    j   0

    init_fill_loop_x:
        MOV     R1      current_data_char_ptr   4
        MUL     R2      i   datawidth   4
        ADD     R2      R2  j           4
        MUL     R2      R2  FOUR        4
        ADD     R1      R1  R2          4

        ;MUL     vga_i   i   FIVE    4
        ;MUL     vga_j   j   FIVE    4
        ;MOVC    R3      0
        ;loop4x:
        ;    MOVC    R4      0
        ;    loop4y:
                LEA     current_VGA_char_ptr   VGA_data

                MUL     R2  i   width   4
                ADD     R2  R2  j   4
                MUL     R2  R2  FOUR    4

                ADD     current_VGA_char_ptr   current_VGA_char_ptr R2  4
        ;        
                $MOV    current_VGA_char_ptr    R1  FOUR

        ;        ADD     R4      R4      ONE     4
        ;        ADD     vga_j   vga_j   ONE 4

        ;        LT      FLAG1   R4      FIVE    4
        ;        $CLEA   F1_ptr  IP      loop4y
        ;    ADD     vga_i   vga_i   ONE 4
        ;    ADD     R3      R3      ONE     4

        ;    LT      FLAG1   R3      FIVE    4
        ;    $CLEA   F1_ptr  IP      loop4x
        
        ADD     j       j       ONE     4

        LT      FLAG1   j       datawidth   4
        $CLEA   F1_ptr  IP      init_fill_loop_x

    ADD     i       i       ONE         4
    LT      FLAG1   i       dataheight      4
    $CLEA   F1_ptr  IP      init_fill_loop_y


    MUL     R1      datawidth   dataheight  4
    MUL     R1      R1      FOUR    4

    OUT     0x1     current_data_char_ptr   4
    ADD current_data_char_ptr current_data_char_ptr R1 4

    MUL     R1      R1      FIVE    4
    MUL     R1      R1      FIVE    4
    LEA     R3      VGA_data
    OUT     0x1     R1   4
    $OUT    TWO      R3      R1

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

datawidth:
    NOP ; zeros
    NOP
    NOP
    NOP

dataheight:
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

