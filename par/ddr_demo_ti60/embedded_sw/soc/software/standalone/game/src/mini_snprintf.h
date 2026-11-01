#ifndef MINI_SNPRINTF_H
#define MINI_SNPRINTF_H
/*
 * Tiny snprintf for bare-metal Sapphire SoC.
 * newlib's snprintf pulls in malloc/_sbrk (not provided by this BSP) and ~20 KB
 * of code, so render.c uses this instead.
 * Supported: %d %i %u %x %X %c %s %% with optional '0' flag and field width
 * (e.g. %02d %03d %8s). No precision, no long / float.
 */
#include <stddef.h>
#include <stdarg.h>

int mini_vsnprintf(char *buf, size_t size, const char *fmt, va_list ap);
int mini_snprintf(char *buf, size_t size, const char *fmt, ...);

#ifdef snprintf
#undef snprintf
#endif
#define snprintf mini_snprintf

#endif
