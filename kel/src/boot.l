GLOBAL 0x0000
$_init
    sig out *$welcome_msg 0
    sig abort 1

$_data
    $welcome_msg STR "Hello, world!\n"
