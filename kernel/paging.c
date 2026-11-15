#include "paging.h"
#include "defines.h"
#include "trap.h"
#include "utils.h"
#include <stddef.h>
#include <stdint.h>

extern char __free_ram[], __free_ram_end[];
extern char __kernel_base[];

static paddr_t next_free_page = 0;

paddr_t kernel_pagetable_root;

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
  // map trampoline page:
  map_page((uint64_t *)root_table, TRAMPOLINE, (paddr_t)__trampoline,
           PTE_R | PTE_X);

  for (paddr_t i = (paddr_t)__kernel_base; i < (paddr_t)__free_ram_end;
       i += PAGE_SIZE) {
    // TODO: different sections of memory should be mapped with different
    // permission bits
    map_page((uint64_t *)root_table, i, i, PTE_R | PTE_W | PTE_X);
  }

  kernel_pagetable_root = root_table;
  write_csr(satp, pagetable_root_2_satp(kernel_pagetable_root));

  // flush tlb
  __asm__ __volatile__("sfence.vma zero, zero" ::: "memory");
  return root_table;
}

// User page table initialization, we don't allocate any (usable) memory, only
// set up the root table returns 0 if error, otherwise the new root table
// pointer
paddr_t u_init_pagetable() {
  paddr_t out_root_table = alloc_zeroed_page();
  if (!out_root_table) {
    panic("Out of memory! Tried to allocate new page table but failed!");
  }

  return out_root_table;
}

// allocates `n_pages` pages and maps them to virtual addresses starting from
// the `base_address` for the kernel.
// returns `base_address` if we allocated correctly, otherwise 0
// NOTE: `base_address` must be aligned to the page size.
// The difference between this and `u_alloc_pages` is flags isn't OR'd with
// PTE_U
vaddr_t alloc_pages(uint64_t *root_table, vaddr_t base_address, size_t n_pages,
                    size_t flags) {
  if (!is_aligned(base_address, PAGE_SIZE)) {
    return 0;
  }

  for (paddr_t i = 0; i < n_pages; ++i) {
    paddr_t new_page = alloc_zeroed_page();
    if (!new_page) {
      panic("Out of memory! Tried to allocate a new page but failed!");
    }
    map_page(root_table, (i * PAGE_SIZE) + base_address, new_page, flags);
  }

  return base_address;
}

// allocates `n_pages` pages and maps them to virtual addresses starting from
// the `base_address`
// returns `base_address` if we allocated correctly, otherwise 0
//
// NOTE: `base_address` must be aligned to the page size and flags is already
// OR'd with PTE_U
inline vaddr_t u_alloc_pages(uint64_t *root_table, vaddr_t base_address,
                             size_t n_pages, size_t flags) {
  // inline function since its just a fancy caller
  return alloc_pages(root_table, base_address, n_pages, flags | PTE_U);
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
  if (table[page_number] & PTE_V) {
    panic("Tried to map the same physical address to two virtual addresses!");
  }
  table[page_number] = ((paddr >> 12) << 10) | flags | PTE_V;
}
