#ifndef _LASM_STYLE_H
#define _LASM_STYLE_H

#define FBOLD    "\x1b[1m"
#define FITALIC  "\x1b[3m"
#define FUNDER   "\x1b[4m"
#define FRED     "\x1b[31m"
#define FGREEN   "\x1b[32m"
#define FYELLOW  "\x1b[33m"
#define FBLUE    "\x1b[34m"
#define FMAGENTA "\x1b[35m"
#define FCYAN    "\x1b[36m"
#define FRESET   "\x1b[0m"

#define LERR(msg) FBOLD "lasm: " FRED "Error: " FRESET msg
#define LERRL(msg) LERR(FBOLD "(line %d) " FRESET) msg
#define LWARN(msg) FBOLD "lasm: " FYELLOW "Warning: " FRESET msg
#define LWARNL(msg) LWARN(FBOLD "(line %d) " FRESET) msg

#endif
