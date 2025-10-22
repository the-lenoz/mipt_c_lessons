    LEA F1_ptr  FLAG1
    LEA current_data_ptr VIDEO_DATA


    MOVC    R1  44100
    MOVC    R2  1000
    MOVC    BUF_SIZE  4410
    MUL     R2  R2  BUF_SIZE  4 
    DIV     SL  R2  R1  4

    MOVC    width   256
    MOVC    height  144

loop1:
    MOV     R3      SL  4
    MOVC    R4      2   ;   порог для остатка от деления (мс)
    MOVC    R5      1

    IN  0x0 R1  4           ; получаем время с запуска в миллисекундах

    DIV R2  R1  R3  4
    MUL R2  R2  R3  4
    SUB R1  R1  R2  4       ; остаток от деления на длину буфера сэмплов в миллисекундах

    LT  FLAG1   R4  R1  4

    CMOV    FLAG1   next_frame   R5  4

    $CLEA   F1_ptr  IP  wait_old_frame  ; ждём окончания проигрывания загруженного буфера

    LT      FLAG1   next_frame   R5  4
    $CLEA   F1_ptr  IP  wait_old_frame   ; только что уже загрузили буфер, пропускаем

    ; загружаем новый буфер
    MOVC    R1  0x3
    MOVC    R3  4
    MUL     R3  BUF_SIZE    R3  4
    $OUT    R1  current_data_ptr  R3

    MOVC    next_frame   0

    ADD current_data_ptr current_data_ptr R3  4

    
    ; загружаем новый кадр

    MOVC    R1  4
    MUL     R3  width   height  4

    MUL     R3  R3      R1      4
    
    MOVC    R1  0x2
    $OUT    R1  current_data_ptr    R3

    ADD     current_data_ptr    current_data_ptr    R3  4


    wait_old_frame:

    $LEA    IP  loop1


IP:
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

R5:
    NOP
    NOP
    NOP
    NOP

next_frame:
    NOP
    NOP
    NOP
    NOP

SL:
    NOP
    NOP
    NOP
    NOP

BUF_SIZE:
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

width:
    NOP
    NOP
    NOP
    NOP

height:
    NOP
    NOP
    NOP
    NOP

current_data_ptr:
    NOP
    NOP
    NOP
    NOP

VIDEO_DATA:
