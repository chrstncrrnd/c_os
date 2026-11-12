#include "proc.h"
#include "defines.h"
#include "paging.h"
#include "trap.h"
#include "utils.h"
#include <stdint.h>

// returns the new process' PID
int create_process(void) {
  struct proc *proc = NULL;
  int i;
  for (i = 0; i < PROCS_MAX; i++) {
    if (procs[i].state == PROC_UNUSED) {
      proc = &procs[i];
      break;
    }
  }

  if (!proc) {
    panic("Couldn't find space for proc process!");
  }
  // PID starts at 0 so that procs[pid] is the correct process
  proc->pid = i;
  proc->page_table_root = u_init_pagetable();
  // allocate for stack
  u_alloc_pages((uint64_t *)proc->page_table_root, K_STACK_BASE_ADDR,
                K_STACK_PAGES, PTE_R | PTE_W);

  proc->state = PROC_RUNNABLE;
  proc->kstack = (uint64_t *)(K_STACK_BASE_ADDR + K_STACK_SIZE);
  proc->trapframe.kernel_sp = (uint64_t)proc->kstack;

  // allocate trampoline
  map_page((uint64_t *)proc->page_table_root, TRAMPOLINE, (paddr_t)__trampoline,
           PTE_R | PTE_W | PTE_X | PTE_U);
  return proc->pid;
}

void init_pcb(void) {
  for (size_t i = 0; i < PROCS_MAX; ++i) {
    procs[i] = (struct proc){
        .pid = -1,
        .state = PROC_UNUSED,
    };
  }
}
