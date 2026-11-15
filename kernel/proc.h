#pragma once

#include "defines.h"
#include "trap.h"
#include "utils.h"
#include <stdint.h>

// states for a given process
enum proc_state {
  PROC_UNUSED,
  PROC_RUNNABLE,
  PROC_RUNNING,
};

struct proc {
  int pid;
  enum proc_state state;
  paddr_t page_table_root;     // program page table
  uint64_t *kernel_stack_base; // kernel stack pointer
  struct trap_frame *trapframe;
};

int create_process(void);
void yield(void);
void init_pcb(void);
void start_process(int pid);

extern struct proc procs[PROCS_MAX];
