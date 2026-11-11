#pragma once
#include <stddef.h>

#define va_list __builtin_va_list
#define va_start __builtin_va_start
#define va_end __builtin_va_end
#define va_arg __builtin_va_arg

#define read_csr(reg)                                                          \
  ({                                                                           \
    size_t __tmp;                                                              \
    __asm__ __volatile__("csrr %0, " #reg : "=r"(__tmp));                      \
    __tmp;                                                                     \
  })

#define write_csr(reg, value)                                                  \
  do {                                                                         \
    size_t __tmp = (value);                                                    \
    __asm__ __volatile__("csrw " #reg ", %0" ::"r"(__tmp));                    \
  } while (0)

#define panic(fmt, ...)                                                        \
  do {                                                                         \
    printf("Kernel panicked: %s:%d: " fmt "\n", __FILE__,                      \
           __LINE__, ##__VA_ARGS__);                                           \
    while (1) {                                                                \
    }                                                                          \
  } while (0)

#define is_aligned(value, align) __builtin_is_aligned(value, align)
#define align_up(value, align) __builtin_align_up(value, align)

typedef __UINTPTR_TYPE__ paddr_t; // physical address
typedef __UINTPTR_TYPE__ vaddr_t; // virtual address

void *memset(void *start, char val, size_t n_bytes);
void *memcpy(char *dest, const char *src, size_t n);

size_t strlen(const char *str);

void printf(const char *fmt, ...);
void init_print(void);
