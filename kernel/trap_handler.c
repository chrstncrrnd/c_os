#include "trap.h"
#include "utils.h"

void trap_handler(void) {
  uint32_t scause = read_csr(scause);
  uint32_t stval = read_csr(stval);
  uint32_t user_pc = read_csr(sepc);

  panic("unexpected trap: scause=%x, stval=%x, sepc=%x\n", scause, stval,
        user_pc);
}
