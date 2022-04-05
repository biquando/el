// [Term]
//   Terminate the current process.

GLOBAL $_TERM_GL
nop $_TERM_ID

$_main
    // return value -> ra
    INC rsp
    POP ra

    // header 3,4 -> rsp
    load # rar $_GL_SPACE_HEAD
    load * rx
    load + rar
    INC rar
    load * rsp
    TRA rx ra

    // Decrement depth offset by 2
    load * ra *$_GL_SPACE_HEAD
    DEC ra
    store * ra

    // Return to parent process
    RET
