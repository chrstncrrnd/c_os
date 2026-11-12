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
  printf("WE GOT HERE!\n");
  int pid = create_process();
  map_page((uint64_t *)procs[pid].page_table_root, (vaddr_t)test_main,
           (paddr_t)test_main, PTE_R | PTE_X | PTE_U);

  printf("POST MAP PAGE!\n");
  uint64_t s = read_csr(sstatus);
  s &= ~(1UL << 8);
  s |= (1UL << 5);
  write_csr(sstatus, s);

  write_csr(stvec, (size_t)user_vec);
  write_csr(sepc, (size_t)test_main);
  write_csr(sscratch, (size_t)procs[pid].kstack);
  switch_pagetable(procs[pid].page_table_root);
  __asm__ __volatile__("sret\n");
}

void kernel_main(void) {
  // set bss section to 0
  memset(__bss, 0, (size_t)__bss_end - (size_t)__bss);

  init_trap();   // initialize interrupt handling
  init_print();  // initialize printing
  init_pcb();    // initialize process control block
  init_paging(); // initialize paging

  printf("Booting C_OS!\n");

  test_prog_runner();
  // __asm__ __volatile__("unimp");

  for (;;) {
    __asm__ __volatile__("nop");
  }
}
