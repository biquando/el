// [GetLocal]
//   Gets the address of the current process's local space body.
// Returns:
//   loc    address of the currrent local space body

GLOBAL $_GET_LOCAL_GL
nop $_GET_LOCAL_ID

load # rar $_GL_SPACE_HEAD
load * rx
load + rar
load # rx 4
mod rx + rar

RET