#include "syscall.h"
#include "proc.h"
#include "utils.h"
#include <stdint.h>

uint64_t sys_print() {
  printf("Print systemcall!\n");
  return 0;
}

static uint64_t (*syscalls[])(void) = {[SYS_PRINT] = (sys_print)};

void handle_syscall() {
  int num = current_proc->trapframe->a7;

  if (num >= 0 && num < n_elems(syscalls) && syscalls[num]) {
    current_proc->trapframe->a0 = (syscalls[num])();
  } else {
    printf("Syscall error: %d is not a valid syscall. Caused by process with "
           "id: %d\n",
           num, current_proc->pid);
    current_proc->trapframe->a0 = -1;
  }
}
