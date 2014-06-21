#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "quid.h"

int mem_seed = MEM_SEED_CYCLE;
int rnd_seed = RND_SEED_CYCLE;

/* read seed or create if not exist */
void get_mem_seed(uuid_node_t *node){
	static int mem_seed_count = 0;
	static uuid_node_t saved_node;
	char seed[16];
	FILE *fp;

	if(!mem_seed_count){
		fp = fopen(RANDFILE, "rb");
		if(fp){
			fread(&saved_node, sizeof saved_node, 1, fp);
			fclose(fp);
		}else{
			seed[0] |= 0x01;
			memcpy(&saved_node, seed, sizeof(saved_node));

			fp = fopen(RANDFILE, "wb");
			if(fp){
				fwrite(&saved_node, sizeof(saved_node), 1, fp);
				fclose(fp);
			}
		}
	}
	if(mem_seed_count == mem_seed){
		mem_seed_count = 0;
	}else{
		mem_seed_count++;
	}

	*node = saved_node;
}

void get_system_time(uuid_time_t *uuid_time){
#ifdef __WIN32___
	ULARGE_INTEGER time;

	GetSystemTimeAsFileTime((FILETIME *)&time);
	time.QuadPart += (unsigned __int64) (1000*1000*10) * (unsigned __int64) (60 * 60 * 24) * (unsigned __int64) (17+30+31+365*18+5);
	*uuid_time = time.QuadPart;
#else
	struct timeval tv;
	unsigned long long result = EPOCH_DIFF;
	gettimeofday(&tv, NULL);
	result += tv.tv_sec;
	result *= 10000000LL;
	result += tv.tv_usec * 10;
	*uuid_time = result;
#endif
}

/* construct QUID */
int quid_create(cuuid_t *uuid){
	uuid_time_t timestamp;
	unsigned short clockseq;
	uuid_node_t node;

	get_current_time(&timestamp);
	get_mem_seed(&node);
	clockseq = true_random();

	format_quid(uuid, clockseq, timestamp, node);

	return 1;
}

/* make a QUID from the timestamp, clockseq, and node ID */
void format_quid(cuuid_t* uuid, unsigned short clock_seq, uuid_time_t timestamp, uuid_node_t node){
	uuid->time_low = (unsigned long)(timestamp & 0xFFFFFFFF);
	uuid->time_mid = (unsigned short)((timestamp >> 32) & 0xFFFF);

	uuid->time_hi_and_version = (unsigned short)((timestamp >> 48) & 0xFFF);
	uuid->time_hi_and_version ^= 0x80;
	uuid->time_hi_and_version |= 0xa000;

	uuid->clock_seq_low = (clock_seq & 0xFF);
	uuid->clock_seq_hi_and_reserved = (clock_seq & 0x3F00) >> 8;
	uuid->clock_seq_hi_and_reserved |= 0x80;

	memcpy(&uuid->node, &node, sizeof(uuid->node));
	uuid->node[0] = 0xec; // Class
	uuid->node[1] = 0x16; // Category
	uuid->node[5] = (true_random() & 0xFF);
}

void get_current_time(uuid_time_t *timestamp){
	static int inited = 0;
	static uuid_time_t time_last;
	static unsigned short uuids_this_tick;
	uuid_time_t time_now;

	if(!inited){
		get_system_time(&time_now);
		uuids_this_tick = UUIDS_PER_TICK;
		inited = 1;
	}

	for(;;){
		get_system_time(&time_now);

		if(time_last != time_now){
			uuids_this_tick = 0;
			time_last = time_now;
			break;
		}
		if(uuids_this_tick < UUIDS_PER_TICK){
			uuids_this_tick++;
			break;
		}
	}

	*timestamp = time_now + uuids_this_tick;
}

double get_tick_count(){
#ifdef __WIN32___
	return GetTickCount();
#else
	struct timespec now;
	if(clock_gettime(CLOCK_MONOTONIC, &now)){
		return 0;
	}

	return now.tv_sec * 1000.0 + now.tv_nsec / 1000000.0;
#endif
}

static unsigned short true_random(){
	static int rnd_seed_count = 0;
	uuid_time_t time_now;

	if(!rnd_seed_count){
		get_system_time(&time_now);
		time_now = time_now / UUIDS_PER_TICK;
		srand((unsigned int)(((time_now >> 32) ^ time_now) & 0xffffffff));
	}
	if(rnd_seed_count == rnd_seed){
		rnd_seed_count = 0;
	}else{
		rnd_seed_count++;
	}

	return (rand()+get_tick_count());
}

/* print QUID */
void quid_print(cuuid_t u){
	printf("{%.8x-", (unsigned int)u.time_low);
	printf("%.4x-", u.time_mid);
	printf("%.4x-", u.time_hi_and_version);
	printf("%x", u.clock_seq_hi_and_reserved);
	printf("%.2x-", u.clock_seq_low);

	printf("%.2x", u.node[0]); // Class
	printf("%.2x", u.node[1]); // Category
	printf("%.2x", u.node[2]);
	printf("%.2x", u.node[3]);
	printf("%.2x", u.node[4]); // Random note
	printf("%.2x", u.node[5]); // Serie

	printf("}\n");
}

/* set memory seed cycle*/
void quid_set_mem_seed(int cnt){
	mem_seed = cnt;
}

/* set rnd seed cycle */
void quid_set_rnd_seed(int cnt){
	rnd_seed = cnt;
}
