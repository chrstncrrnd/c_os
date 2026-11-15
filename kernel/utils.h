#pragma once
#include <stddef.h>

// === REGISTER UTILS ===
#define SSTATUS_SPP (1L << 8)  // Previous mode : 1 = supervisor, 0 = user
#define SSTATUS_SPIE (1L << 5) // Supervisor Previous Interrupt Enable
#define SSTATUS_UPIE (1L << 4) // User Previous Interrupt Enable
#define SSTATUS_SIE (1L << 1)  // Supervisor Interrupt Enable
#define SSTATUS_UIE (1L << 0)  // User Interrupt Enable

#define n_elems(arr) (sizeof(arr) / sizeof((arr)[0]))

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

// csr = csr | rs
#define bit_set_csr(reg, value)                                                \
  do {                                                                         \
    __asm__ __volatile__("csrs " #reg ", %0" ::"rK"(value) : "memory");        \
  } while (0)

// csr = csr & !rs
#define bit_clear_csr(reg, value)                                              \
  do {                                                                         \
    __asm__ __volatile__("csrc " #reg ", %0" ::"rK"(value) : "memory");        \
  } while (0)

// interrupt utils
static inline void interrupt_enable() { bit_set_csr(sstatus, SSTATUS_SIE); }
static inline void interrupt_disable() { bit_clear_csr(sstatus, SSTATUS_SIE); }

// === KERNEL UTILS ===
#define panic(fmt, ...)                                                        \
  do {                                                                         \
    printf("Kernel panicked: %s:%d: " fmt "\n", __FILE__,                      \
           __LINE__, ##__VA_ARGS__);                                           \
    while (1) {                                                                \
    }                                                                          \
  } while (0)

void *memset(void *start, char val, size_t n_bytes);
void *memcpy(char *dest, const char *src, size_t n);

size_t strlen(const char *str);

void printf(const char *fmt, ...);
void init_print(void);

// === TYPE UTILS ===
#define is_aligned(value, align) __builtin_is_aligned(value, align)
#define align_up(value, align) __builtin_align_up(value, align)

typedef __UINTPTR_TYPE__ paddr_t; // physical address
typedef __UINTPTR_TYPE__ vaddr_t; // virtual address

// === VARIADICS ===
#define va_list __builtin_va_list
#define va_start __builtin_va_start
#define va_end __builtin_va_end
#define va_arg __builtin_va_arg
