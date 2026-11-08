#pragma once
#include "utils.h"

#define SBI_SUCCESS 0                // Completed successfully
#define SBI_ERR_FAILED -1            // Failed
#define SBI_ERR_NOT_SUPPORTED -2     // Not supported
#define SBI_ERR_INVALID_PARAM -3     // Invalid parameter(s)
#define SBI_ERR_DENIED -4            // Denied or not allowed
#define SBI_ERR_INVALID_ADDRESS -5   // Invalid address(s)
#define SBI_ERR_ALREADY_AVAILABLE -6 // Already available
#define SBI_ERR_ALREADY_STARTED -7   // Already started
#define SBI_ERR_ALREADY_STOPPED -8   // Already stopped
#define SBI_ERR_NO_SHMEM -9          // Shared memory not available
#define SBI_ERR_INVALID_STATE -10    // Invalid state
#define SBI_ERR_BAD_RANGE -11        // Bad (or invalid) range
#define SBI_ERR_TIMEOUT -12          // Failed due to timeout
#define SBI_ERR_IO -13               // Input/Output error
#define SBI_ERR_DENIED_LOCKED -14    // Denied or not allowed due to lock status

#define SBI_EXT_DBCN 0x4442434E // DBCN extension number
#define SBI_EXT_BASE 0x10       // base extension number

struct sbiret {
  long error;
  union {
    long value;
    unsigned long uvalue;
  };
};

struct sbiret sbi_call(long arg0, long arg1, long arg2, long arg3, long arg4,
                       long arg5, long fid, long eid);

// === Base extensions ===
struct sbiret sbi_probe_extension(long extension_id);

// === Legacy extensions ===
long sbi_console_putchar(char ch);
long sbi_console_getchar(void);

// === DCBN extensions ===

struct sbiret sbi_debug_console_write(const char *buf, unsigned long len);

struct sbiret sbi_debug_console_read(const char *buf, unsigned long len);

struct sbiret sbi_debug_console_write_byte(uint8_t byte);
