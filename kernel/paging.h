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

#define switch_pagetable(new_pagetable)                                        \
  do {                                                                         \
    write_csr(satp, SATP_SV39_MODE | ((new_pagetable) >> 12));                 \
    __asm__ __volatile__("sfence.vma" ::: "memory");                           \
  } while (0)

paddr_t init_paging(void);

void map_page(uint64_t *root_table, vaddr_t vaddr, paddr_t paddr,
              uint64_t flags);

paddr_t walk(uint64_t *root_table, vaddr_t vaddr);
paddr_t u_init_pagetable(paddr_t base_address, size_t initial_size);
