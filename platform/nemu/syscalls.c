// See LICENSE for license details.

#include <stdint.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <limits.h>
#include <sys/signal.h>
#include "util.h"


#undef strcmp
#define static_assert(cond) switch(0) { case 0: case !!(long)(cond): ; }
#define nemu_trap(code) asm volatile("mv a0, %0; .word 0x0000006b" : :"r"(code))

volatile uint8_t* uartlite = (volatile uint8_t*)0x40600000;

#define UART_LITE_RX_FIFO    0x0
#define UART_LITE_TX_FIFO    0x4
#define UART_LITE_STAT_REG   0x8
#define UART_LITE_CTRL_REG   0xc

#define UART_LITE_RST_FIFO   0x03
#define UART_LITE_INTR_EN    0x10
#define UART_LITE_TX_FULL    0x08
#define UART_LITE_TX_EMPTY   0x04
#define UART_LITE_RX_FULL    0x02
#define UART_LITE_RX_VALID   0x01

void uartlite_putchar(uint8_t ch) {
  if (ch == '\n') uartlite_putchar('\r');

  while (uartlite[UART_LITE_STAT_REG] & UART_LITE_TX_FULL);
  uartlite[UART_LITE_TX_FIFO] = ch;
}

size_t strlen(const char *s)
{
  const char *p = s;
  while (*p)
    p++;
  return p - s;
}

size_t strnlen(const char *s, size_t n)
{
  const char *p = s;
  while (n-- && *p)
    p++;
  return p - s;
}

#define NUM_COUNTERS 2
static uintptr_t counters[NUM_COUNTERS];
static char* counter_names[NUM_COUNTERS];

void setStats(int enable)
{
  int i = 0;
#define READ_CTR(name) do { \
    while (i >= NUM_COUNTERS) ; \
    uintptr_t csr = read_csr(name); \
    if (!enable) { csr -= counters[i]; counter_names[i] = #name; } \
    counters[i++] = csr; \
  } while (0)

  READ_CTR(mcycle);
  READ_CTR(minstret);

#undef READ_CTR
}

void exit(int code)
{
  nemu_trap(0);
}

#undef putchar
int putchar(int ch)
{
  uartlite_putchar(ch);
}

static inline void printnum(void (*putch)(int, void**), void **putdat,
                    unsigned long long num, unsigned base, int width, int padc)
{
  unsigned digs[sizeof(num)*CHAR_BIT];
  int pos = 0;

  while (1)
  {
    digs[pos++] = num % base;
    if (num < base)
      break;
    num /= base;
  }

  while (width-- > pos)
    putch(padc, putdat);

  while (pos-- > 0)
    putch(digs[pos] + (digs[pos] >= 10 ? 'a' - 10 : '0'), putdat);
}

static long long getint(va_list *ap, int lflag)
{
  if (lflag >= 2)
    return va_arg(*ap, long long);
  else if (lflag)
    return va_arg(*ap, long);
  else
    return va_arg(*ap, int);
}

static unsigned long long getuint(va_list *ap, int lflag)
{
  if (lflag >= 2)
    return va_arg(*ap, unsigned long long);
  else if (lflag)
    return va_arg(*ap, unsigned long);
  else
    return va_arg(*ap, unsigned int);
}


static void vprintfmt(void (*putch)(int, void**), void **putdat, const char *fmt, va_list ap)
{
  register const char* p;
  const char* last_fmt;
  register int ch, err;
  unsigned long long num;
  int base, lflag, width, precision, altflag;
  char padc;

  while (1) {
    while ((ch = *(unsigned char *) fmt) != '%') {
      if (ch == '\0')
        return;
      fmt++;
      putch(ch, putdat);
    }
    fmt++;

    // Process a %-escape sequence
    last_fmt = fmt;
    padc = ' ';
    width = -1;
    precision = -1;
    lflag = 0;
    altflag = 0;
  reswitch:
    switch (ch = *(unsigned char *) fmt++) {

    // flag to pad on the right
    case '-':
      padc = '-';
      goto reswitch;
      
    // flag to pad with 0's instead of spaces
    case '0':
      padc = '0';
      goto reswitch;

    // width field
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
      for (precision = 0; ; ++fmt) {
        precision = precision * 10 + ch - '0';
        ch = *fmt;
        if (ch < '0' || ch > '9')
          break;
      }
      goto process_precision;

    case '*':
      precision = va_arg(ap, int);
      goto process_precision;

    case '.':
      if (width < 0)
        width = 0;
      goto reswitch;

    case '#':
      altflag = 1;
      goto reswitch;

    process_precision:
      if (width < 0)
        width = precision, precision = -1;
      goto reswitch;

    // long flag (doubled for long long)
    case 'l':
      lflag++;
      goto reswitch;

    // character
    case 'c':
      putch(va_arg(ap, int), putdat);
      break;

    // string
    case 's':
      if ((p = va_arg(ap, char *)) == NULL)
        p = "(null)";
      if (width > 0 && padc != '-')
        for (width -= strnlen(p, precision); width > 0; width--)
          putch(padc, putdat);
      for (; (ch = *p) != '\0' && (precision < 0 || --precision >= 0); width--) {
        putch(ch, putdat);
        p++;
      }
      for (; width > 0; width--)
        putch(' ', putdat);
      break;

    // (signed) decimal
    case 'd':
      num = getint(&ap, lflag);
      if ((long long) num < 0) {
        putch('-', putdat);
        num = -(long long) num;
      }
      base = 10;
      goto signed_number;

    // unsigned decimal
    case 'u':
      base = 10;
      goto unsigned_number;

    // (unsigned) octal
    case 'o':
      // should do something with padding so it's always 3 octits
      base = 8;
      goto unsigned_number;

    // pointer
    case 'p':
      static_assert(sizeof(long) == sizeof(void*));
      lflag = 1;
      putch('0', putdat);
      putch('x', putdat);
      /* fall through to 'x' */

    // (unsigned) hexadecimal
    case 'x':
      base = 16;
    unsigned_number:
      num = getuint(&ap, lflag);
    signed_number:
      printnum(putch, putdat, num, base, width, padc);
      break;

    // escaped '%' character
    case '%':
      putch(ch, putdat);
      break;
      
    // unrecognized escape sequence - just print it literally
    default:
      putch('%', putdat);
      fmt = last_fmt;
      break;
    }
  }
}

void printstr(const char* s)
{
  for (int i = 0; i < strlen(s); i++){
    uartlite_putchar(s[i]);
  }
  
}

int puts(const char* s)
{
  printstr(s);
  printstr("\n");
  return strlen(s);
}

int printf(const char* fmt, ...)
{
  va_list ap;
  va_start(ap, fmt);

  vprintfmt((void*)putchar, 0, fmt, ap);

  va_end(ap);
  return 0; // incorrect return value, but who cares, anyway?
}

int sprintf(char* str, const char* fmt, ...)
{
  va_list ap;
  char* str0 = str;
  va_start(ap, fmt);

  void sprintf_putch(int ch, void** data)
  {
    char** pstr = (char**)data;
    **pstr = ch;
    (*pstr)++;
  }

  vprintfmt(sprintf_putch, (void**)&str, fmt, ap);
  *str = 0;

  va_end(ap);
  return str - str0;
}

void* memcpy(void* dest, const void* src, size_t len)
{
  if ((((uintptr_t)dest | (uintptr_t)src | len) & (sizeof(uintptr_t)-1)) == 0) {
    const uintptr_t* s = src;
    uintptr_t *d = dest;
    while (d < (uintptr_t*)(dest + len))
      *d++ = *s++;
  } else {
    const char* s = src;
    char *d = dest;
    while (d < (char*)(dest + len))
      *d++ = *s++;
  }
  return dest;
}

void* memset(void* dest, int byte, size_t len)
{
  if ((((uintptr_t)dest | len) & (sizeof(uintptr_t)-1)) == 0) {
    uintptr_t word = byte & 0xFF;
    word |= word << 8;
    word |= word << 16;
    word |= word << 16 << 16;

    uintptr_t *d = dest;
    while (d < (uintptr_t*)(dest + len))
      *d++ = word;
  } else {
    char *d = dest;
    while (d < (char*)(dest + len))
      *d++ = byte;
  }
  return dest;
}

int strcmp(const char* s1, const char* s2)
{
  unsigned char c1, c2;

  do {
    c1 = *s1++;
    c2 = *s2++;
  } while (c1 != 0 && c1 == c2);

  return c1 - c2;
}

char* strcpy(char* dest, const char* src)
{
  char* d = dest;
  while ((*d++ = *src++))
    ;
  return dest;
}

long atol(const char* str)
{
  long res = 0;
  int sign = 0;

  while (*str == ' ')
    str++;

  if (*str == '-' || *str == '+') {
    sign = *str == '-';
    str++;
  }

  while (*str) {
    res *= 10;
    res += *str++ - '0';
  }

  return sign ? -res : res;
}

void _init(){

}