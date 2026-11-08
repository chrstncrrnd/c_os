#include "utils.h"

extern char __bss[], __bss_end[];
extern char __stack_top[];
extern char __free_ram[], __free_ram_end[];
extern char __kernel_base[];

void *memset(void *start, char val, size_t n_bytes) {
  uint8_t *buf_bytes = (uint8_t *)start;
  for (size_t i = 0; i < n_bytes; ++i) {
    buf_bytes[i] = val;
  }
  return start;
}

void kernel_main(void) { memset(__bss, 0, (size_t)__bss_end - (size_t)__bss); }

__attribute__((section(".text.boot"))) __attribute__((naked)) void boot(void) {
  __asm__ __volatile__("mv sp, %[stack_top]\n"
                       "j kernel_main"
                       :
                       : [stack_top] "r"(__stack_top));
}
