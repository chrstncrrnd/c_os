#pragma once
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

static inline uint64_t pagetable_root_2_satp(uint64_t page_table_root) {
  return (SATP_SV39_MODE | (page_table_root >> 12));
}

#define switch_pagetable(new_pagetable)                                        \
  do {                                                                         \
    write_csr(satp, pagetable_root_2_satp(new_pagetable));                     \
    __asm__ __volatile__("sfence.vma" ::: "memory");                           \
  } while (0)

paddr_t init_paging(void);

paddr_t alloc_zeroed_page();
void map_page(uint64_t *root_table, vaddr_t vaddr, paddr_t paddr,
              uint64_t flags);

paddr_t walk(uint64_t *root_table, vaddr_t vaddr);
paddr_t u_init_pagetable();
vaddr_t u_alloc_pages(uint64_t *root_table, vaddr_t base_address,
                      size_t n_pages, size_t flags);
vaddr_t k_alloc_pages(vaddr_t base_address, size_t n_pages, size_t flags);

// kernel root table pointer
extern paddr_t kernel_pagetable_root;
