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

// extract the 9 bit page number at level from vaddr
inline uint64_t extract_page_number(vaddr_t vaddr, size_t level) {
  return (vaddr >> (12 + 9 * level)) & 0x1FF;
}

inline uint64_t *pte_2_addr(uint64_t pte) {
  return (uint64_t *)(((pte >> 10) & ((1ULL << 44) - 1)) << 12);
}

// Caller is responsible for ORing this with the correct flags
inline uint64_t paddr_2_pte(paddr_t paddr) {
  return ((paddr >> 12) << 10) | PTE_V;
}

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

// TODO: keep track of free pages.
// free the page at paddr
void free_page(uint64_t *paddr) {}

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
    table = pte_2_addr(pte);
  }
  uint64_t page_number = (vaddr >> 12) & 0x1FF;
  return (table[page_number] >> 10) << 12;
}

// internal recursive function for freeing a specific page
// returns true if the page was freed. The idea is that if we call this and it
// returns true, we should clear its pte
bool __internal_unmap_page(uint64_t *pagetable, vaddr_t vaddr, size_t level) {
  // base case, we are at the level where the pte is for the actual page
  if (level == -1) {
    // clearly unmapping a page should not cause it to be freed, hence we just
    // set its pte to 0, this is done by the caller.
    return true;
  }
  uint64_t page_number = extract_page_number(vaddr, level);
  uint64_t pte = pagetable[page_number];
  // we don't have a valid page table entry
  if (!(pte & PTE_V)) {
    // invalid reference, we can return false since it has been cleared already
    return false;
  }
  // guard
  if ((pte & (PTE_R | PTE_W | PTE_X)) && level > 0) {
    panic("(UNIMPLEMENTED) map_page hit a leaf at level: %d", level);
  }
  // go one level deeper
  uint64_t *new_table = pte_2_addr(pte);

  // if the page was cleared, we must clear the page table entry
  if (__internal_unmap_page(new_table, vaddr, level - 1)) {
    // freeing the data page is left to the caller
    if (level > 0) {
      free_page(new_table);
    }
    pagetable[page_number] = 0;
  } else {
    // this is at least one valid entry in this page table so we can be sure
    // that its not empty
    return false;
  }

  // each pte is 64 bits, and we have a whole page of them.
  size_t n_ptes = (PAGE_SIZE / sizeof(uint64_t));

  // scan to see if the page is now empty, now that we have gotten rid of that
  // entry
  for (size_t i = 0; i < n_ptes; ++i) {
    // we have found a valid pte, hence we cannot clear this table
    if (pagetable[i] & PTE_V) {
      return false;
    }
  }
  return true;
}

// unmap the page starting at vaddr.
// Cleans up unused page tables, however if the root_table is empty, it is not
// cleaned up.
//
// NOTE: requires vaddr to be aligned to PAGE_SIZE
void unmap_page(uint64_t *root_table, vaddr_t vaddr) {
  __internal_unmap_page(root_table, vaddr, 2);
  __asm__ __volatile__("sfence.vma %0, zero" ::"r"(vaddr) : "memory");
}

// sv39 page mapping function.
//
// Maps the virtual page starting at vaddr to the physical page starting at
// paddr.
//
// TODO: add references here to read more.
//
// NOTE: requires vaddr and paddr to be aligned to PAGE_SIZE
void map_page(uint64_t *root_table, vaddr_t vaddr, paddr_t paddr,
              uint64_t flags) {

  if (!(is_aligned(vaddr, PAGE_SIZE) && is_aligned(paddr, PAGE_SIZE))) {
    panic("Tried to map an unaligned page!");
  }
  uint64_t *table = root_table;
  for (size_t level = 2; level > 0; --level) {
    uint64_t page_number = extract_page_number(vaddr, level);
    uint64_t pte = table[page_number];
    // we don't have a valid page table entry
    if (!(pte & PTE_V)) {
      // allocate a new page
      paddr_t new_page = alloc_zeroed_page();
      pte = paddr_2_pte(new_page);
      // write our new page table entry
      table[page_number] = pte;
      table = (uint64_t *)new_page;
      continue;
    }
    if (pte & (PTE_R | PTE_W | PTE_X)) {
      panic("(UNIMPLEMENTED) map_page hit a leaf at level: %d", level);
    }
    // go one level deeper
    table = pte_2_addr(pte);
  }
  // finally, write our pte to the lowest level
  uint64_t page_number = extract_page_number(vaddr, 0);
  if (table[page_number] & PTE_V) {
    panic("Tried to map the same physical address to two virtual addresses!");
  }
  table[page_number] = paddr_2_pte(paddr) | flags;
}
