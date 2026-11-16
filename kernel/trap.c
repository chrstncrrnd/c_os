#include "trap.h"
#include "defines.h"
#include "paging.h"
#include "proc.h"
#include "syscall.h"
#include "utils.h"
#include <stddef.h>
#include <stdint.h>

void kernel_trap(void) {
  uint64_t scause = read_csr(scause);
  uint64_t stval = read_csr(stval);
  uint64_t user_pc = read_csr(sepc);

  // from supervisor mode
  panic("recieved trap while in kernel mode: scause=%x, stval=%x, sepc=%x\n",
        scause, stval, user_pc);
}

// from user mode
uint64_t user_trap(void) {
  uint64_t scause = read_csr(scause);
  uint64_t stval = read_csr(stval);
  uint64_t user_pc = read_csr(sepc);
  current_proc->trapframe->epc = user_pc;

  write_csr(stvec, (uint64_t)kernel_vec);

  // 1 bit to indicate if we had an interrupt
  bool interrupt = (scause >> 63);
  if (interrupt) {
    panic("unexpected trap (interrupt): scause=%x, stval=%x, sepc=%x\n", scause,
          stval, user_pc);
  }

  switch (scause) {
  case 8:
    // ECALL
    current_proc->trapframe->epc += 4;
    interrupt_enable();
    handle_syscall();
    break;
  case 12:
  case 13:
  case 15: // page faults
    break;
  default:
    panic("unexpected trap: scause=%x stval=%x sepc=%x", scause, stval,
          user_pc);
  }

  prepare_user_return();

  return pagetable_root_2_satp(current_proc->page_table_root);
}

void user_enter() {
  prepare_user_return();
  uint64_t trampoline_userret =
      TRAMPOLINE + ((uint64_t)userret - (uint64_t)trampoline_start);
  ((void (*)(uint64_t))trampoline_userret)(
      pagetable_root_2_satp(current_proc->page_table_root));
}

void prepare_user_return() {
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
  current_proc->trapframe->kernel_satp =
      read_csr(satp); // kernel page table root
  current_proc->trapframe->kernel_sp =
      (uint64_t)(current_proc
                     ->kernel_stack_base); // process's kernel stack (we set the
                                           // stack pointer back to the base)
  current_proc->trapframe->kernel_trap = (uint64_t)user_trap;

  // return to user mode
  unsigned long x = read_csr(sstatus);
  x &= ~SSTATUS_SPP; // user mode flag
  x |= SSTATUS_SPIE; // user mode interrupts
  write_csr(sstatus, x);

  // set S Exception Program Counter to the saved user pc.
  write_csr(sepc, current_proc->trapframe->epc);
}

// writes the kernel vec to stvec (note that we will need to change to the user
// trap handler when starting a user process)
void init_trap(void) {
  // write the corresponding register
  write_csr(stvec, (size_t)kernel_vec);
}
