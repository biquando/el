// fib.l
//   Calculate the fibonacci sequence.
// Args:
//   1: addr  addr of dest array
//   2: n     number of elements
// Returns:
//   argument 2, 'n'

GLOBAL $FIB_L
nop $FIB_ID
$_init
    // Load 'n' into ra
    load # rar 4
    mod rar + rsp
    load * ra
    load # rb 1
    mod ra ? rb
    cond l
        GOTO $return

    // Set rd to 2 * 'n'
    load # rd 0
    mod rd + ra
    mod rd + ra

    // Load 'addr' into rar
    DEC rar
    load * rar

    // Initialize array
    load # rx 2
    store * rb
    mod ra ? rb
    cond ng
        GOTO $return
    store + rb

$loop
    // Add previous two numbers
    DEC rx
    load + rb
    INC rx
    load + rc
    INC rx
    mod rb + rc

    // Exit if finished, otherwise store result and loop
    mod rx ? rd
    cond nl
        GOTO $return
    store + rb
    GOTO $loop

$return
    TRA rx ra
    RET
