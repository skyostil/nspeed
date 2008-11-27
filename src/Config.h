#ifndef CONFIG_H
#define CONFIG_H

/* Compiler and platform specific settings */
#if defined(HAVE_CONFIG_H)
#  include "AutoConfig.h"
#endif

#if defined(__GNUC__)
#    define PACKED                      __attribute__((packed)) 
#    define PREFETCH(addr,p1,p2)        __builtin_prefetch(addr, p1, p2)
#    define RESTRICT                    __restrict__ 
#elif defined(_MSC_VER)
#    define PACKED
#    define snprintf _snprintf
#    define ZLIB_WINAPI
#    define PREFETCH(addr, p1, p2)
#    define RESTRICT
#else
#    error Unsupported compiler
#endif

#endif
