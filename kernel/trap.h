#pragma once
#include "utils.h"

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

extern void trap_entry(void);
void trap_handler(void);
