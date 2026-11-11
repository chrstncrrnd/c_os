#include "paging.h"
#include "defines.h"
#include "utils.h"
#include <stddef.h>
#include <stdint.h>

extern char __free_ram[], __free_ram_end[];
extern char __kernel_base[];

static paddr_t next_free_page;

paddr_t kernel_root_table;

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

paddr_t init_paging(void) {
  paddr_t root_table = alloc_zeroed_page();
  if (!root_table) {
    panic("Out of memory! Tried to allocate kernel root table but failed.");
  }
  for (paddr_t i = (paddr_t)__kernel_base; i < (paddr_t)__free_ram_end;
       i += PAGE_SIZE) {
    // TODO: different sections of memory should be mapped with different
    // permission bits
    map_page((uint64_t *)root_table, i, i, PTE_R | PTE_W | PTE_X);
  }

  kernel_root_table = root_table;
  write_csr(satp, SATP_SV39_MODE | (kernel_root_table >> 12));

  // flush tlb
  __asm__ __volatile__("sfence.vma" ::: "memory");
  return root_table;
}

// NOTE: requires initial_size and base_address to be aligned to PAGE_SIZE
// User page table initialization, we allocate initial_size bytes
// returns 0 if error, otherwise the new root table pointer
paddr_t u_init_pagetable(paddr_t base_address, size_t initial_size) {
  if (!(is_aligned(base_address, PAGE_SIZE) &&
        is_aligned(initial_size, PAGE_SIZE))) {
    return 0;
  }
  paddr_t out_root_table = alloc_zeroed_page();
  if (!out_root_table) {
    panic("Out of memory! Tried to allocate new page table but failed!");
  }

  for (paddr_t i = 0; i < initial_size; i += PAGE_SIZE) {
    paddr_t new_page = alloc_zeroed_page();
    if (!new_page) {
      panic("Out of memory! Tried to allocate new page table but failed!");
    }
    // TODO: make sure we map each section of the program accordingly
    map_page((uint64_t *)out_root_table, i + base_address, new_page,
             PTE_R | PTE_W | PTE_X | PTE_U);
  }
  return out_root_table;
}

paddr_t walk(uint64_t *root_table, vaddr_t vaddr) {
  if (!is_aligned(vaddr, PAGE_SIZE)) {
    panic("Tried to walk to an unaligned page!");
  }
  uint64_t *table = root_table;
  for (size_t level = 2; level > 0; --level) {
    uint64_t page_number = (vaddr >> (12 + 9 * level)) & 0x1FF;
    uint64_t pte = table[page_number];
    // we don't have a valid page table entry
    if (!(pte & PTE_V)) {
      return 0;
    }
    if (pte & (PTE_R | PTE_W | PTE_X)) {
      panic("(UNIMPLEMENTED) map_page hit a leaf at level: %d", level);
    }
    // go one level deeper
    table = (uint64_t *)(((pte >> 10) & ((1ULL << 44) - 1)) << 12);
  }
  uint64_t page_number = (vaddr >> 12) & 0x1FF;
  return (table[page_number] >> 10) << 12;
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
