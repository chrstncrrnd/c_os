#include "trap.h"
#include "utils.h"
#include <stddef.h>
#include <stdint.h>

// stub for dev work
void ecall(void) { panic("Got ecall"); }

// stub for dev work
void timer(void) { panic("got timer"); }

// stub for dev work
void external(void) { panic("got external"); }

void user_trap(void) {
  // from user mode
}

void kernel_trap(void) {
  // from supervisor mode
}

void trap_handler(void) {
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

void init_trap(void) {
  // write the corresponding register
  write_csr(stvec, (size_t)user_vec);
}
