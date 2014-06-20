#ifdef __WIN32__
#include <unistd.h>
#include <winsock2.h>
#else
#include <unistd.h>
#include <sys/time.h>
#endif
#ifdef DEBUG
#include <valgrind/valgrind.h>
#endif

#ifndef __MAIN__
#define __MAIN__

typedef struct {
	unsigned long   time_low;
	unsigned short  time_mid;
	unsigned short  time_hi_and_version;
	unsigned char   clock_seq_hi_and_reserved;
	unsigned char   clock_seq_low;
	unsigned char   node[6];
} cuuid_t;

extern int uuid_create(cuuid_t *);
extern void quid_print(cuuid_t);

#endif
