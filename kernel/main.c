#include "paging.h"
#include "trap.h"
#include "utils.h"

extern char __bss[], __bss_end[];
extern char __stack_top[];
extern char __free_ram[], __free_ram_end[];
extern char __kernel_base[];

void kernel_main(void) {
  // set bss section to 0
  memset(__bss, 0, (size_t)__bss_end - (size_t)__bss);

  init_trap();   // initialize interrupt handling
  init_paging(); // initialize paging
  init_print();  // initialize printing

  printf("Booting C_OS!\n");

  __asm__ __volatile__("unimp");

  for (;;) {
    __asm__ __volatile__("nop");
  }
}
