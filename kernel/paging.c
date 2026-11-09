#include "paging.h"
#include "utils.h"
#include <stddef.h>
#include <stdint.h>

#define PTE_V (1 << 0)
#define PTE_R (1 << 1)
#define PTE_W (1 << 2)
#define PTE_X (1 << 3)
#define PTE_U (1 << 4)
#define PTE_G (1 << 5)
#define PTE_A (1 << 6)
#define PTE_D (1 << 7)
#define SATP_SV39_MODE (8ULL << 60)
#define PAGE_SIZE 4096

extern char __free_ram[], __free_ram_end[];
extern char __kernel_base[];

static paddr_t next_free_page;

paddr_t alloc_zeroed_page() {
  if (next_free_page == 0) {
    next_free_page = (paddr_t)__free_ram;
  }

  if (next_free_page + PAGE_SIZE > (paddr_t)__free_ram_end) {
    return 0;
  }

  paddr_t page_start = next_free_page;

  memset((void *)page_start, 0, PAGE_SIZE);
  next_free_page += PAGE_SIZE;
  return page_start;
}

void init_paging(void) {
  paddr_t root_table = alloc_zeroed_page();
  if (!root_table) {
    panic("Out of memory! Tried to allocate root table but failed.");
  }
  for (paddr_t i = (paddr_t)__kernel_base; i < (paddr_t)__free_ram_end;
       i += PAGE_SIZE) {
    map_page((uint64_t *)root_table, i, i,
             PTE_R | PTE_W | PTE_X | PTE_A | PTE_D);
  }

  write_csr(satp, SATP_SV39_MODE | (root_table >> 12));

  __asm__ __volatile__("sfence.vma" ::: "memory");
}

void map_page(uint64_t *root_table, vaddr_t vaddr, paddr_t paddr,
              uint64_t flags) {

  if (!(is_aligned(vaddr, PAGE_SIZE) && is_aligned(paddr, PAGE_SIZE))) {
    panic("Tried to map unaligned pages!");
  }
  uint64_t *table = root_table;
  for (size_t level = 2; level > 0; --level) {
    uint64_t page_number = (vaddr >> (12 + 9 * level)) & 0x1FF;
    uint64_t pte = table[page_number];
    // we don't have a valid page table entry
    if (!(pte & PTE_V)) {
      // allocate a new page
      paddr_t new_page = alloc_zeroed_page();
      pte = ((new_page >> 12) << 10) | PTE_V;
      // write our new page table entry
      table[page_number] = pte;
      table = (uint64_t *)new_page;
      continue;
    }
    if (pte & (PTE_R | PTE_W | PTE_X)) {
      panic("(UNIMPLEMENTED) map_page hit a leaf at level: %d", level);
    }
    // go one level deeper
    table = (uint64_t *)(((pte >> 10) & ((1ULL << 44) - 1)) << 12);
  }
  // finally, write our pte to the lowest level
  uint64_t page_number = (vaddr >> 12) & 0x1FF;
  table[page_number] = ((paddr >> 12) << 10) | flags | PTE_V;
}
