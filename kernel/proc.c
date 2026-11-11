#include "proc.h"
#include "defines.h"
#include "paging.h"
#include "utils.h"
#include <stdint.h>

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

  proc->pid = i + 1;
  proc->page_table_root = u_init_pagetable(0, 10);
  proc->ctx = (struct context){
      .ra = 0,
      .sp = 0,
      .s0 = 0,
      .s1 = 0,
      .s2 = 0,
      .s3 = 0,
      .s4 = 0,
      .s5 = 0,
      .s6 = 0,
      .s7 = 0,
      .s8 = 0,
      .s9 = 0,
      .s10 = 0,
      .s11 = 0,
  };
  proc->state = PROC_RUNNABLE;
  proc->kstack = return i;
}

void init_pcb(void) {
  for (size_t i = 0; i < PROCS_MAX; ++i) {
    procs[i] = (struct proc){
        .pid = -1,
        .state = PROC_UNUSED,
    };
  }
}
