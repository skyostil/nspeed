#ifndef CONFIG_H
#define CONFIG_H

#ifdef _MSC_VER
#define PACKED
#else
#define PACKED __attribute__((packed)) 
#endif

#define EPOC

#ifndef NULL
#define NULL	(0)
#endif

#endif
