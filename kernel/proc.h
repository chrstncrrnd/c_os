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

struct context {
  uint64_t ra; // return address
  uint64_t sp; // stack pointer (saved to the stack ;)

  // callee-saved
  uint64_t s0;
  uint64_t s1;
  uint64_t s2;
  uint64_t s3;
  uint64_t s4;
  uint64_t s5;
  uint64_t s6;
  uint64_t s7;
  uint64_t s8;
  uint64_t s9;
  uint64_t s10;
  uint64_t s11;
};

struct proc {
  int pid;
  enum proc_state state;
  paddr_t page_table_root;     // program page table
  uint64_t *kernel_stack_base; // kernel stack pointer
  char *text_start;
  struct trap_frame *trapframe;
};

int create_process(void);
extern void switch_context(struct context *old_ctx, struct context *new_ctx);
void yield(void);
void init_pcb(void);
void start_process(int pid);

extern struct proc procs[PROCS_MAX];
