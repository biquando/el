// [Spawn]
//   Spawn a sub-process.
// Args:
//   1. addr        Address of the process routine
//   2. size        Size in bytes of the local space body
//   3. proc_args*  Arguments to pass into the process routine

GLOBAL $_SPAWN_GL
nop $_SPAWN_ID

$_main
    // 'addr' - 3 -> rc
    load # rx 2
    TRA rar rsp
    load + rc
    load # rx 3
    mod rc - rx

    // 'size' + 4 -> ra
    load # rx 4
    load + ra
    mod ra + rx

    // ra -> local head 1,2
    load # rar $_GL_SPACE_HEAD
    load * rx
    load + rar
    load * rx
    mod rar + rx
    store * ra

    // rsp + 2 -> local head 3,4
    TRA ra rsp
    load # rx 2
    mod ra + rx
    store + ra
    TRA rb rar

    // Update global header
    load # rar $_GL_SPACE_HEAD
    load * rx
    INC rx
    store * rx
    store + rb

    // Set word[rsp + 2] to word[rsp]
    TRA rar rsp
    load * rb
    INC rar
    store * rb
    INC rsp

    // Set rip to 'addr' - 3
    mod rc - rip
    load # rx 9
    mod rc - rx
    mod rip + rc
