
    MOVC    ONE     1
    MOVC    B       15
    MOVC    N       16
    MOVC    n       -10
    LEA     F1_ptr  FLAG1
    LT      FLAG1   n       N       4


loop1:
    ADD     SUM     SUM     B       4
    SUB     N       N       ONE     4
    LT      FLAG1   n       N       4
    OUT     0x0     N
    $CLEA   F1_ptr  IP      loop1
;loop1 end
    OUT     0x0     SUM
    HLT          



; DATA
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

B:    
    NOP ; zeros
    NOP
    NOP
    NOP

SUM:    
    NOP ; zeros
    NOP
    NOP
    NOP

N:
    NOP ; zeros
    NOP
    NOP
    NOP

ONE:
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

