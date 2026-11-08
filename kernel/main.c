#include "utils.h"

extern char __bss[], __bss_end[];
extern char __stack_top[];
extern char __free_ram[], __free_ram_end[];
extern char __kernel_base[];

void kernel_main(void) {
  memset(__bss, 0, (size_t)__bss_end - (size_t)__bss);
  printf("Hello, world!");
  for (;;) {
    __asm__ __volatile__("nop");
  }
}

__attribute__((section(".text.boot"))) void boot(void) {
  __asm__ __volatile__("lla sp, __stack_top\n"
                       "j kernel_main\n");
}
