#include "syscall.h"
#include "proc.h"
#include "utils.h"
#include <stdint.h>

uint64_t sys_print() {
  printf("Print systemcall with a0: %d!\n", current_proc->trapframe->a0);
  return 0;
}

uint64_t sys_yield() {
  yield();
  return 0;
}

static uint64_t (*syscalls[])(void) = {[SYS_PRINT] = (sys_print),
                                       [SYS_YIELD] = (sys_yield)};

void handle_syscall() {
  int num = current_proc->trapframe->a7;

  if (num >= 0 && (size_t)num < n_elems(syscalls) && syscalls[num]) {
    current_proc->trapframe->a0 = (syscalls[num])();
  } else {
    printf("Syscall error: %d is not a valid syscall. Caused by process with "
           "id: %d\n",
           num, current_proc->pid);
    current_proc->trapframe->a0 = -1;
  }
}
