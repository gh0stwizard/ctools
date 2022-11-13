#ifndef PERRORF_H
#define PERRORF_H

#include <stdarg.h>

extern int
vperrorf(const char *fmt, va_list ap);

extern int
perrorf(const char *fmt, ...);

#endif /* PERRORF_H */
