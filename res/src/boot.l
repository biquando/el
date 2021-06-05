// GLOBAL 0x0000
$_init
    sig out *$msg 0
    load * rsp *$bsp

    // If a program is loaded at 0x1100, run it
    load * ra *0x1100
    load # rb 0xFF
    mod ra ? rb
    cond ne
        GOTO $skip_0x1100
    CALL $proc_0x1100
    $skip_0x1100

    sig abort 1

$proc_0x1100
    sig out *$msg_0x1100 0
    load * ra *$n
    PSH ra
    load * ra *$addr
    PSH ra
    CALL 0x1100
    
    POP rb
    POP ra
    mod ra + ra
    load # rx 0

    $loop_0x1100
        mod rx ? ra
        cond nl
            GOTO $break_0x1100
        load # rar 0
        mod rar | rb
        sig out 2
        INC rb
        INC rx
        GOTO $loop_0x1100

    $break_0x1100

    RET

$_data
    $msg STR "Successfully booted.\n"
    $msg_0x1100 STR "Calling procedure 0x1100\n"
    $bsp INT 0xF000
    $n INT 25
    $addr INT 0x2000
