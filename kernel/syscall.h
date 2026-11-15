#pragma once

// === SYSCALL NUMBERS ===
// dummy system call for debugging
// null terminated string
#define SYS_PRINT 0

// defines for syscall.c:
void handle_syscall(void);
