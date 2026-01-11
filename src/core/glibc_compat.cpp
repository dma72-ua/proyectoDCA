#include <dlfcn.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

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
  typedef long int (*strtol_func)(const char *, char **, int);
  static strtol_func real_strtol = NULL;

  if (!real_strtol) {
    real_strtol = (strtol_func)dlsym(RTLD_NEXT, "strtol");
  }

  return real_strtol(nptr, endptr, base);
}

// Wrapper for __isoc23_strtoul
unsigned long int __isoc23_strtoul(const char *nptr, char **endptr, int base) {
  typedef unsigned long int (*strtoul_func)(const char *, char **, int);
  static strtoul_func real_strtoul = NULL;

  if (!real_strtoul) {
    real_strtoul = (strtoul_func)dlsym(RTLD_NEXT, "strtoul");
  }

  return real_strtoul(nptr, endptr, base);
}
}