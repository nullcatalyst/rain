#include <limits.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdlib.h>

int vsnprintf(char* str, size_t size, const char* format, va_list arg);

// `sprintf` is NOT recommended to use, use `snprintf` instead.
//
// Accidental incorrect use `sprintf` may cause buffer overflow, since this is the same as calling
// `snprintf` with `SIZE_T_MAX` as the buffer size (as seen below), and there is absolutely no way
// that your buffer is that large.
int sprintf(char* str, const char* format, ...) {
    va_list args;
    va_start(args, format);
    int ret = vsnprintf(str, SIZE_MAX, format, args);
    va_end(args);
    return ret;
}
