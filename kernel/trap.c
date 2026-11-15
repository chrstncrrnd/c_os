#include "trap.h"
#include "defines.h"
#include "paging.h"
#include "proc.h"
#include "utils.h"
#include <stddef.h>
#include <stdint.h>

// stub for dev work
void ecall(void) { panic("Got ecall"); }

// stub for dev work
void timer(void) { panic("got timer"); }

// stub for dev work
void external(void) { panic("got external"); }

void kernel_trap(void) {
  uint64_t scause = read_csr(scause);
  uint64_t stval = read_csr(stval);
  uint64_t user_pc = read_csr(sepc);

  // from supervisor mode
  panic("recieved trap while in kernel mode: scause=%x, stval=%x, sepc=%x\n",
        scause, stval, user_pc);
}

// from user mode
void user_trap(void) {
  uint64_t scause = read_csr(scause);
  uint64_t stval = read_csr(stval);
  uint64_t user_pc = read_csr(sepc);

  // 1 bit to indicate if we had an interrupt
  bool interrupt = (scause >> 63);
  if (!interrupt) {
    panic("unexpected trap: scause=%x, stval=%x, sepc=%x\n", scause, stval,
          user_pc);
  }

  switch (scause) {
  case 1:
    ecall();
    break;
  case 5:
    timer();
    break;
  case 9:
    external();
  }
}

void user_return(int pid) {
  struct proc p = procs[pid];

  // we're about to switch the destination of traps from
  // kerneltrap() to usertrap(). because a trap from kernel
  // code to usertrap would be a disaster, turn off interrupts.
  interrupt_disable();

  // enable user_vec as our trap interceptor
  uint64_t trampoline_uservec =
      TRAMPOLINE + ((uint64_t)user_vec - (uint64_t)trampoline_start);
  write_csr(stvec, trampoline_uservec);

  // set up trapframe values that uservec will need when
  // the process next traps into the kernel
  p.trapframe.kernel_satp = read_csr(satp); // kernel page table root
  p.trapframe.kernel_sp =
      (uint64_t)(p.kernel_stack_base); // process's kernel stack (we set the
                                       // stack pointer back to the base)
  p.trapframe.kernel_trap = (uint64_t)user_trap;

  // return to user mode
  unsigned long x = read_csr(sstatus);
  x &= ~SSTATUS_SPP; // user mode flag
  x |= SSTATUS_SPIE; // user mode interrupts
  write_csr(sstatus, x);

  // set S Exception Program Counter to the saved user pc.
  write_csr(sepc, p.trapframe.epc);
  uint64_t trampoline_userret =
      TRAMPOLINE + ((uint64_t)userret - (uint64_t)trampoline_start);
  ((void (*)(uint64_t))trampoline_userret)(
      pagetable_root_2_satp(p.page_table_root));
}

// writes the kernel vec to stvec (note that we will need to change to the user
// trap handler when starting a user process)
void init_trap(void) {
  // write the corresponding register
  write_csr(stvec, (size_t)kernel_vec);
}
