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

  proc->state = PROC_RUNNABLE;
  proc->kernel_stack_base = (uint64_t *)(alloc_zeroed_page() + PAGE_SIZE);
  // map trampoline
  map_page((uint64_t *)proc->page_table_root, TRAMPOLINE, (paddr_t)__trampoline,
           PTE_R | PTE_X);
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
