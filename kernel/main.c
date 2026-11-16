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
extern void test_main2(void);

void test_prog_runner(void) {
  // process 1
  int pid = create_process();
  struct proc *p1 = &procs[pid];
  // allocate text section
  u_alloc_pages((uint64_t *)p1->page_table_root, 0x40000, 1, PTE_X | PTE_R);
  // find allocated text section
  paddr_t text_section = walk((uint64_t *)p1->page_table_root, 0x40000);
  // copy program to text section
  memcpy((char *)text_section, (char *)test_main, PAGE_SIZE);
  p1->trapframe->epc = 0x40000;

  // process 2
  int pid2 = create_process();
  struct proc *p2 = &procs[pid2];
  // allocate text section
  u_alloc_pages((uint64_t *)p2->page_table_root, 0x40000, 1, PTE_X | PTE_R);
  // find allocated text section
  paddr_t text_section2 = walk((uint64_t *)p2->page_table_root, 0x40000);
  // copy program to text section
  memcpy((char *)text_section2, (char *)test_main2, PAGE_SIZE);
  p2->trapframe->epc = 0x40000;

  current_proc = p1;
  current_proc->state = PROC_RUNNING;
  user_enter();
}

void kernel_main(void) {
  // set bss section to 0
  memset(__bss, 0, (size_t)__bss_end - (size_t)__bss);

  init_trap();                           // initialize interrupt handling
  init_print();                          // initialize printing
  init_pcb();                            // initialize process control block
  kernel_pagetable_root = init_paging(); // initialize paging

  printf("Booting C_OS!\n");

  test_prog_runner();
  // __asm__ __volatile__("unimp");

  for (;;) {
    __asm__ __volatile__("nop");
  }
}
