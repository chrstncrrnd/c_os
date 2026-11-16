#pragma once

// === SYSCALL NUMBERS ===
// dummy system call for debugging
#define SYS_PRINT 0

#define SYS_YIELD 1

// defines for syscall.c:
void handle_syscall(void);
