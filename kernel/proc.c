#include "proc.h"
#include "defines.h"
#include "paging.h"
#include "trap.h"
#include "utils.h"
#include <stdint.h>

struct proc *current_proc;

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

  // TODO: change this to take a variable number of pages
  proc->kernel_stack_base = (uint64_t *)(alloc_zeroed_page() + PAGE_SIZE);
  // map trampoline page
  map_page((uint64_t *)proc->page_table_root, TRAMPOLINE, (paddr_t)__trampoline,
           PTE_R | PTE_X);

  // allocate trap frame page (only supervisor can read and write from this),
  // it needs to be mapped for the user (since we read/write to it in supervisor
  // mode under the user pagetable)
  alloc_pages((uint64_t *)proc->page_table_root, TRAP_FRAME, 1, PTE_R | PTE_W);
  // since we have an identity map over all physical memory, we can access this
  // process' trap frame via its physical address
  proc->trapframe =
      (struct trap_frame *)walk((uint64_t *)proc->page_table_root, TRAP_FRAME);

  // set up context
  memset(&proc->context, 0, sizeof proc->context);
  proc->context.ra = (uint64_t)user_enter;
  proc->context.sp =
      (uint64_t)proc->kernel_stack_base; // top of the kernel stack

  u_alloc_pages((uint64_t *)proc->page_table_root, U_STACK_TOP - U_STACK_SIZE,
                U_STACK_PAGES, PTE_R | PTE_W);

  proc->trapframe->sp = U_STACK_TOP;
  proc->state = PROC_RUNNABLE;
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

void yield(void) {
  struct proc *next;
  for (int i = 0; i < PROCS_MAX; ++i) {
    if (procs[i].state == PROC_RUNNABLE && procs[i].pid >= 0) {
      next = &procs[i];
      break;
    }
  }
  if (next == current_proc) {
    return;
  }

  struct proc *previous = current_proc;
  current_proc = next;

  previous->state = PROC_RUNNABLE;
  current_proc->state = PROC_RUNNING;
  switch_context(&previous->context, &current_proc->context);
}
