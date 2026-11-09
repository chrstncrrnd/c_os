#pragma once

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

typedef __UINT8_TYPE__ uint8_t;
typedef __UINT16_TYPE__ uint16_t;
typedef __UINT32_TYPE__ uint32_t;
typedef __UINT64_TYPE__ uint64_t;

typedef __SIZE_TYPE__ size_t;
typedef __UINTPTR_TYPE__ uintptr_t;

void *memset(void *start, char val, size_t n_bytes);
size_t strlen(const char *str);

void printf(const char *fmt, ...);
void init_print(void);
