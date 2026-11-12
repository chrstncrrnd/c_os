#pragma once
#include <stdint.h>

// external symbol to where the actual trampoline starts
extern char trampoline_start[];

struct trap_frame {
  uint64_t kernel_satp;    // kernel page table
  uint64_t kernel_sp;      // top of process's kernel stack
  uint64_t kernel_trap;    // usertrap()
  uint64_t epc;            // saved user program counter
  uint64_t _kernel_hartid; // saved kernel tp (not used currently but needed for
                           // memory alignment)
  uint64_t ra;
  uint64_t sp;
  uint64_t gp;
  uint64_t tp;
  uint64_t t0;
  uint64_t t1;
  uint64_t t2;
  uint64_t s0;
  uint64_t s1;
  uint64_t a0;
  uint64_t a1;
  uint64_t a2;
  uint64_t a3;
  uint64_t a4;
  uint64_t a5;
  uint64_t a6;
  uint64_t a7;
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
  uint64_t t3;
  uint64_t t4;
  uint64_t t5;
  uint64_t t6;
};

// implementation in trap.c
void user_trap(void);
void kernel_trap(void);
void init_trap(void);
void user_return(int pid);

// defined in trap_entry.S
extern void kernel_vec(void);

// defined in trampoline.S
extern void user_vec(struct trap_frame *);
extern void userret(uint64_t satp);
