#include <unistd.h>
#include <sys/time.h>
#ifdef DEBUG
#include <valgrind/valgrind.h>
#endif

#ifndef __MAIN__
#define __MAIN__

#define GL_DELAY 15

typedef struct {
	unsigned long   time_low;
	unsigned short  time_mid;
	unsigned short  time_hi_and_version;
	unsigned char   clock_seq_hi_and_reserved;
	unsigned char   clock_seq_low;
	unsigned char   node[6];
} cuuid_t;

extern int quid_create(cuuid_t *);
extern void quid_print(cuuid_t);
extern void quid_set_rnd_seed(int);
extern void quid_set_mem_seed(int);

#endif
