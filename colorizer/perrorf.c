#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include <assert.h>


#ifndef PERRORF_BUFSIZE
#define PERRORF_BUFSIZE 4096
#endif
#ifndef PERRORF_BUFFER_FACTOR
#define PERRORF_BUFFER_FACTOR 2
#endif


extern int
vperrorf(const char *fmt, va_list ap)
{
    int n;
    char *data;
    size_t size = PERRORF_BUFSIZE;


    if (fmt == NULL || strlen(fmt) == 0)
        return 0;

    data = malloc(sizeof(*data) * size);
    assert(data != NULL);

    n = vsnprintf(data, size, fmt, ap);

    if (n < 0) {
        free(data);
        return n;
    }

    while (n >= size - 1) {
        printf("realloc n = %i size = %zu\n", n, size);
        size *= PERRORF_BUFFER_FACTOR;
        data = realloc(data, size);
        assert(data != NULL);
        va_list apcopy;
        va_copy(apcopy, ap);
        n = vsnprintf(data, size, fmt, apcopy);
    }

    perror(data);

    return n;
}


extern int
perrorf(const char *fmt, ...)
{
    int r;
    va_list ap;
    va_start(ap, fmt);
    r = vperrorf(fmt, ap);
    va_end(ap);
    return r;
}
