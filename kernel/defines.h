#pragma once
#define PROCS_MAX 8

#define PAGE_SIZE 4096

#define MAX_VADDR (1UL << 38)

#define K_STACK_SIZE 4096
#define K_STACK_PAGES (K_STACK_SIZE / PAGE_SIZE)

// memory layout (credit to xv6 :) )
#define TRAMPOLINE (MAX_VADDR - PAGE_SIZE)
#define TRAP_FRAME (TRAMPOLINE - PAGE_SIZE)
