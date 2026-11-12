#define PROCS_MAX 8

#define PAGE_SIZE 4096

#define MAX_VADDR ((1 << 38) - 1)

#define K_STACK_SIZE 4096
#define K_STACK_PAGES (K_STACK_SIZE / PAGE_SIZE)
// NOTE: the kernel stack grows from K_STACK_BASE_ADDR + K_STACK_SIZE to
// K_STACK_BASE_ADDR
#define K_STACK_BASE_ADDR 4096
