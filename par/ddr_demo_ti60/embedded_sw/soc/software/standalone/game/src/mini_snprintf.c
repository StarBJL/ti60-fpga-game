#include "mini_snprintf.h"

typedef struct { char *buf; size_t size, pos; } Out;

static void put(Out *o, char c) {
    if (o->size && o->pos + 1 < o->size) o->buf[o->pos] = c;
    o->pos++;
}

static void put_str(Out *o, const char *s, int len, int width, char pad) {
    int i;
    for (i = len; i < width; i++) put(o, pad);
    for (i = 0; i < len; i++) put(o, s[i]);
}

int mini_vsnprintf(char *buf, size_t size, const char *fmt, va_list ap) {
    Out o; o.buf = buf; o.size = size; o.pos = 0;
    for (; *fmt; fmt++) {
        char pad = ' ', tmp[12];
        int width = 0, len = 0;
        if (*fmt != '%') { put(&o, *fmt); continue; }
        fmt++;
        if (*fmt == '0') { pad = '0'; fmt++; }
        while (*fmt >= '0' && *fmt <= '9') width = width * 10 + (*fmt++ - '0');
        switch (*fmt) {
        case 'd': case 'i': {
            int v = va_arg(ap, int);
            unsigned u = v < 0 ? 0u - (unsigned)v : (unsigned)v;
            do { tmp[len++] = (char)('0' + u % 10u); u /= 10u; } while (u);
            if (v < 0) {
                if (pad == '0') { put(&o, '-'); if (width) width--; }
                else tmp[len++] = '-';
            }
            { char r[12]; int i; for (i = 0; i < len; i++) r[i] = tmp[len - 1 - i];
              put_str(&o, r, len, width, pad); }
            break;
        }
        case 'u': case 'x': case 'X': {
            unsigned u = va_arg(ap, unsigned), base = *fmt == 'u' ? 10u : 16u;
            const char *dig = *fmt == 'x' ? "0123456789abcdef" : "0123456789ABCDEF";
            do { tmp[len++] = dig[u % base]; u /= base; } while (u);
            { char r[12]; int i; for (i = 0; i < len; i++) r[i] = tmp[len - 1 - i];
              put_str(&o, r, len, width, pad); }
            break;
        }
        case 'c': tmp[0] = (char)va_arg(ap, int); put_str(&o, tmp, 1, width, ' '); break;
        case 's': {
            const char *s = va_arg(ap, const char *);
            if (!s) s = "(null)";
            while (s[len]) len++;
            put_str(&o, s, len, width, ' ');
            break;
        }
        case '%': put(&o, '%'); break;
        case 0: fmt--; break;
        default: put(&o, '%'); put(&o, *fmt); break;
        }
    }
    if (size) buf[o.pos < size ? o.pos : size - 1] = 0;
    return (int)o.pos;
}

int mini_snprintf(char *buf, size_t size, const char *fmt, ...) {
    int n; va_list ap;
    va_start(ap, fmt);
    n = mini_vsnprintf(buf, size, fmt, ap);
    va_end(ap);
    return n;
}
