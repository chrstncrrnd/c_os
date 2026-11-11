#pragma once

#include "defines.h"
#include "utils.h"
#include <stdint.h>

// saved registers for context switching
struct context {
  uint64_t ra;
  uint64_t sp;

  // callee-saved registers
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

// states for a given process
enum proc_state {
  PROC_UNUSED,
  PROC_RUNNABLE,
  PROC_RUNNING,
};

struct proc {
  int pid;
  enum proc_state state;
  paddr_t page_table_root; // program page table
  vaddr_t ksp;             // kernel stack pointer (not kerbal space program)
  struct context ctx;
};

void create_process(struct proc procs[PROCS_MAX], uint64_t pc);
