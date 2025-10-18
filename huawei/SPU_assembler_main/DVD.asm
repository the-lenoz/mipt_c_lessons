
    LEA     F1_ptr  FLAG1
    LT      FLAG1   n       N       4


    MOVC    FILLC   0x00ffffff
    MOVC    EMPTYC  0x00000000


    MOVC    width   1280
    MOVC    height  720
    MOVC    radius  8
    
    MOVC    ONE     1
    MOVC    TWO     2
    MOVC    FOUR    4

    MOVC    R1      2

    MOV    vX      ONE  4
    MOV    vY      ONE  4

    DIV     centerX width   R1  4
    DIV     centerY height   R1  4

    OUT     0x1     centerX     4
    OUT     0x1     centerY     4

loopM:
    ADD     centerX     centerX vX     4
    ADD     centerY     centerY vY     4

    LEA    current_VGA_char_ptr    VGA_data
    MOVC    i       0

    loopY:
        MOVC    j       0
        loopX:
            
            LEA     R1      EMPTYC
            $MOV    current_VGA_char_ptr    R1  FOUR


            SUB     R1      i       centerY 4
            MUL     R1      R1      R1      4

            SUB     R2      j       centerX 4
            MUL     R2      R2      R2      4

            MUL     R3      radius  radius  4
            
            ADD     R1      R1      R2      4

            

            LT      FLAG1   R1      R3      4

            LEA     R1      FILLC
            $CMOV   F1_ptr  current_VGA_char_ptr          R1    FOUR

            ADD     j       j       ONE     4

            ADD     current_VGA_char_ptr    current_VGA_char_ptr    FOUR 4

            LT      FLAG1   j       width   4
            $CLEA   F1_ptr  IP      loopX
        ;loopX end

        ADD     i       i       ONE     4
        LT      FLAG1   i       height  4
        $CLEA   F1_ptr  IP      loopY
    ;loopY end

    LT      FLAG1   width   centerX  4
    $CLEA   F1_ptr  IP      change_vX

    LT      FLAG1   centerX ZERO  4
    $CLEA   F1_ptr  IP      change_vX

    $LEA    IP  remain_vX

change_vX:
    SUB     vX      ZERO    vX  4

remain_vX:

    LT      FLAG1   height   centerY  4
    $CLEA   F1_ptr  IP      change_vY

    LT      FLAG1   centerY ZERO  4
    $CLEA   F1_ptr  IP      change_vY

    $LEA    IP  remain_vY

change_vY:
    SUB     vY      ZERO    vY  4

remain_vY:

    MUL     R1      width   height  4
    MUL     R1      R1      FOUR    4
    LEA     R3      VGA_data
    $OUT    TWO      R3      R1

    LT      FLAG1   ONE     width   4
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

i:
    NOP
    NOP
    NOP
    NOP

j:    
    NOP ; zeros
    NOP
    NOP
    NOP

vX:
    NOP
    NOP
    NOP
    NOP

vY:
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
    NOP

