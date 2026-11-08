#include "utils.h"
#include "sbi.h"

static bool dbcn_extension_enabled = false;

void *memset(void *start, char val, size_t n_bytes) {
  uint8_t *buf_bytes = (uint8_t *)start;
  for (size_t i = 0; i < n_bytes; ++i) {
    buf_bytes[i] = val;
  }
  return start;
}

size_t strlen(const char *str) {
  size_t len = 0;
  while (*str) {
    ++len;
    ++str;
  }
  return len;
}

static void __internal_print_char(const char c) {
  if (dbcn_extension_enabled) {
    sbi_debug_console_write_byte(c);
  } else {
    sbi_console_putchar(c);
  }
}

void print_init(void) {
  dbcn_extension_enabled = (sbi_probe_extension(SBI_EXT_DBCN).value != 0);
}

void printf(const char *fmt, ...) {
  va_list vargs;
  va_start(vargs, fmt);

  while (*fmt) {

    if (*fmt == '%') {
      // consume the %
      fmt++;

      switch (*fmt) {
      case '\0':
        __internal_print_char('%');
        goto end;
      case '%':
        // "%%" => print %
        __internal_print_char('%');
        break;
      case 's':;
        // string
        const char *s = va_arg(vargs, const char *);
        while (*s != '\0') {
          __internal_print_char(*(s++));
          ;
        }
        break;
      case 'd':;
        int value = va_arg(vargs, int);
        unsigned mag = value;
        if (value < 0) {
          __internal_print_char('-');
          mag = -mag;
        }

        unsigned divisor = 1;
        while (mag / divisor > 9)
          divisor *= 10;

        while (divisor > 0) {
          __internal_print_char('0' + mag / divisor);
          mag %= divisor;
          divisor /= 10;
        }
        break;
      case 'x':;
        unsigned hx_value = va_arg(vargs, unsigned);
        for (int i = 7; i >= 0; i--) {
          unsigned nibble = (hx_value >> (i * 4)) & 0xf;
          __internal_print_char("0123456789abcdef"[nibble]);
        }
      }
    } else {
      __internal_print_char(*fmt);
    }

    fmt++;
  }

end:
  va_end(vargs);
}
