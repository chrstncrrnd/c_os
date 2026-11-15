#include "syscall.h"
#include "proc.h"
#include <stdint.h>

uint64_t sys_print() {
  printf("Print systemcall!");
  return 0;
}

static uint64_t (*syscalls[])(void) = {[SYS_PRINT] = (sys_print)};

void handle_syscall() { int num = current_proc->trapframe->a7; }
