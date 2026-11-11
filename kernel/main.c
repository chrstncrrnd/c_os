#include "defines.h"
#include "paging.h"
#include "proc.h"
#include "trap.h"
#include "utils.h"
#include <stdint.h>

extern char __bss[], __bss_end[];
extern char __stack_top[];
extern char __free_ram[], __free_ram_end[];
extern char __kernel_base[];

struct proc procs[PROCS_MAX];

void test(int k) { printf("Okay test %d worked!\n", k); }

void kernel_main(void) {
  // set bss section to 0
  memset(__bss, 0, (size_t)__bss_end - (size_t)__bss);

  init_trap();  // initialize interrupt handling
  init_print(); // initialize printing
  paddr_t kernel_pagetable_root = init_paging(); // initialize paging

  printf("Booting C_OS!\n");
  test(1);

  uint64_t new_pagetable = u_init_pagetable(0, 100 * 4096);
  printf("Old: %x, new: %x \n", align_up((paddr_t)test, 4096),
         walk((uint64_t *)kernel_pagetable_root,
              (vaddr_t)align_up((paddr_t)test, 4096)));
  // switch_pagetable(new_pagetable);
  test(2);

  // __asm__ __volatile__("unimp");

  for (;;) {
    __asm__ __volatile__("nop");
  }
}
