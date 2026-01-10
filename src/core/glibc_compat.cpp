#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

extern "C" {

// Wrapper for __isoc23_sscanf
int __isoc23_sscanf(const char *str, const char *format, ...) {
    va_list args;
    va_start(args, format);
    int ret = vsscanf(str, format, args);
    va_end(args);
    return ret;
}

// Wrapper for __isoc23_strtol
long int __isoc23_strtol(const char *nptr, char **endptr, int base) {
    return strtol(nptr, endptr, base);
}

// Wrapper for __isoc23_strtoul
unsigned long int __isoc23_strtoul(const char *nptr, char **endptr, int base) {
    return strtoul(nptr, endptr, base);
}

}
