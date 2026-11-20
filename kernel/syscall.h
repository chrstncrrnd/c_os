#pragma once

// === SYSCALL NUMBERS ===
// dummy system call for debugging
#define SYS_PRINT 0

#define SYS_YIELD 1
#define SYS_EXIT 2

// defines for syscall.c:
void handle_syscall(void);
