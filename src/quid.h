#ifdef __WIN32__
#include <unistd.h>
#include <winsock2.h>
#else
#include <unistd.h>
#include <sys/time.h>
#include <ctype.h>
#endif
#ifdef DEBUG
#include <valgrind/valgrind.h>
#endif

#ifndef __QUID__
#define __QUID__

#define UUIDS_PER_TICK 1024
#define EPOCH_DIFF 11644473600LL
#define RANDFILE ".rnd"
#define MEM_SEED_CYCLE 65536
#define RND_SEED_CYCLE 4096
#define QUID_STRLEN 32

typedef struct {
	unsigned long time_low;
	unsigned short time_mid;
	unsigned short time_hi_and_version;
	unsigned char clock_seq_hi_and_reserved;
	unsigned char clock_seq_low;
	unsigned char node[6];
} cuuid_t;

typedef unsigned long long uuid_time_t;

typedef struct {
	char nodeID[6];
} uuid_node_t;

static void format_quid(cuuid_t *, unsigned short, uuid_time_t, uuid_node_t);
static void get_current_time(uuid_time_t *);
static void get_mem_seed(uuid_node_t *);
static void get_system_time(uuid_time_t *);
static unsigned short true_random();
static double get_tick_count(void);
static unsigned short true_random(void);
int quid_create(cuuid_t *, char, char);
int quid_get_uuid(char *, cuuid_t *);
void quid_print(cuuid_t, int);
void quid_print_file(FILE *, cuuid_t, int);
void quid_set_rnd_seed(int);
void quid_set_mem_seed(int);
void set_flag(cuuid_t *, char);
void set_class(cuuid_t *, char);
void unset_flag(cuuid_t *, char);
void strtouuid(char *, cuuid_t *);

#endif