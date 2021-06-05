// GLOBAL 0x0800
//     GOTO $init
// $init
//     sig out *$startmsg 0
//     load * ra *$fir
//     load * rb *$sec
//     load * rd *$max
//     sig out 2
//     sig out *$fir 2
//     sig out *$sec 2
// $loop
//     load # rc 0
//     mod rc + ra
//     mod rc + rb
//     store * rc *$res
// 
//     cond c
//     GOTO $break
//     mod rc ? rd
//     cond a
//     GOTO $break
// 
//     sig out 2
//     load # ra 0
//     mod ra | rb
//     load # rb 0
//     mod rb | rc
//     GOTO $loop
// $break
//     sig out *$done 0
//     sig abort 1
// $data
//     $fir INT 1
//     $sec INT 1
//     $max INT 10000
//     $res INT 0
//     $startmsg STR "Calculating fibonacci sequence up to "
//     $done STR "Finished sequence\n"

GLOBAL 0x9000
mod ra ++
INC rb
mod rc --
DEC rd
