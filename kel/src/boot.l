// boot.l
// [Boot]
//   Boots the kernel.
GLOBAL $_BOOT_L
nop $_BOOT_ID

$_init
    sig out *$welcome_msg 0
    load # rsp $_STACK

    // Verify [Boot]
    load # rx $_BOOT_ID
    PSH rx
    load # rx $_BOOT_L
    PSH rx
    CALL $_verify
    load # ra 0
    mod ra ? rx
    cond ne
        GOTO $verify_fail

    // Verify fib.l
    load # rx $FIB_ID
    PSH rx
    load # rx $FIB_L
    PSH rx
    CALL $_verify
    load # ra 0
    mod ra ? rx
    cond ne
        GOTO $verify_fail

    GOTO $verify_success
    $verify_fail
        sig out *$verify_fail_msg 0
        PSH rx
        TRA rar rsp
        sig out 2
        sig abort 2
    $verify_success
    sig out *$verify_success_msg 0
    sig abort 1

// Verify that a routine was loaded into memory.
// Args:
//   1. addr  address of the routine
//   2. id    16-bit identifier of the routine
// Returns:
//   0 if success, id if failure
$_verify
    // addr->ra, id->rb
    load # rar 2
    mod rar + rsp
    load * ra
    INC rar
    load * rb
    TRA rx rb
    TRA rar ra

    // Verify 'addr' matches <Global> tag of routine
    mod rar ++
    load * rc
    mod ra ? rc
    cond ne
        GOTO $_verify_return

    // Verify 'id' matches id tag of routine
    mod rar ++
    mod rar ++
    mod rar ++
    load * rc
    mod rb ? rc
    cond ne
        GOTO $_verify_return
    
    // Both tags matched
    load # rx 0
    RET

    $_verify_return
        RET

$_data
    $welcome_msg         STR "Booting...\n"
    $verify_success_msg  STR "Verification succeeded.\n"
    $verify_fail_msg     STR "Verification failed, id="
