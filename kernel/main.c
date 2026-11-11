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

struct proc procs[PROCS_MAX] = {};

extern void test_main(void);

void test_prog_runner(void) {
  int pid = create_process();
  map_page((uint64_t *)procs[pid].page_table_root, (vaddr_t)test_main,
           (paddr_t)test_main, PTE_R | PTE_W | PTE_X | PTE_U);

  switch_pagetable(procs[pid].page_table_root);
  test_main();
}

void kernel_main(void) {
  // set bss section to 0
  memset(__bss, 0, (size_t)__bss_end - (size_t)__bss);

  init_trap();  // initialize interrupt handling
  init_print(); // initialize printing
  init_pcb();   // initialize process control block
  paddr_t kernel_pagetable_root = init_paging(); // initialize paging

  printf("Booting C_OS!\n");

  test_prog_runner();
  // __asm__ __volatile__("unimp");

  for (;;) {
    __asm__ __volatile__("nop");
  }
}
