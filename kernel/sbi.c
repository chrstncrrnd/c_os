#include "sbi.h"
#include "utils.h"

struct sbiret sbi_call(long arg0, long arg1, long arg2, long arg3, long arg4,
                       long arg5, long fid, long eid) {

  // - a7 encodes the SBI extension ID (EID).
  // - a6 encodes the SBI function ID (FID) for a given extension ID encoded in
  // a7 for any SBI extension defined in or after SBI v0.2. a0 through a5
  // contain
  // - the arguments for the SBI function call. Registers that are not defined
  // in the SBI function call are not reserved.
  register long a0 __asm__("a0") = arg0;
  register long a1 __asm__("a1") = arg1;
  register long a2 __asm__("a2") = arg2;
  register long a3 __asm__("a3") = arg3;
  register long a4 __asm__("a4") = arg4;
  register long a5 __asm__("a5") = arg5;
  register long a6 __asm__("a6") = fid;
  register long a7 __asm__("a7") = eid;

  __asm__ __volatile__("ecall"
                       : "=r"(a0), "=r"(a1)
                       : "r"(a0), "r"(a1), "r"(a2), "r"(a3), "r"(a4), "r"(a5),
                         "r"(a6), "r"(a7)
                       : "memory");
  return (struct sbiret){.error = a0, .value = a1};
}

// === Base extensions ===

// Returns 0 if the given SBI extension ID (EID) is not available, or 1 if it is
// available unless defined as any other non-zero value by the implementation
struct sbiret sbi_probe_extension(long extension_id) {
  return sbi_call(extension_id, 0, 0, 0, 0, 0, 3, SBI_EXT_BASE);
}

// === Legacy extensions ===

// Write data present in ch to debug console
long sbi_console_putchar(char ch) {
  return sbi_call((unsigned long)ch, 0, 0, 0, 0, 0, 0, 1).error;
}

// Read a byte from debug console.
long sbi_console_getchar(void) {
  return sbi_call(0, 0, 0, 0, 0, 0, 0, 2).error;
}

// Write bytes to the debug console from input memory.
// Number of bytes written is stored in sbiret.uvalue
struct sbiret sbi_debug_console_write(const char *buf, unsigned long len) {
  return sbi_call(len, (unsigned long)buf, 0, 0, 0, 0, 0, SBI_EXT_DBCN);
}

// read len bytes from the debug console
struct sbiret sbi_debug_console_read(const char *buf, unsigned long len) {
  return sbi_call(len, (unsigned long)buf, 0, 0, 0, 0, 1, SBI_EXT_DBCN);
}
struct sbiret sbi_debug_console_write_byte(uint8_t byte) {

  return sbi_call((unsigned long)byte, 0, 0, 0, 0, 0, 2, SBI_EXT_DBCN);
}
