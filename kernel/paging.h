#pragma once
#include "utils.h"
#include <stddef.h>
#include <stdint.h>

void init_paging(void);

void map_page(uint64_t *root_table, vaddr_t vaddr, paddr_t paddr,
              uint64_t flags);
