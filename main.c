#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "uuid.h"

/* read seed or create if not exist */
void get_mem_seed(uuid_node_t *node){
	static int inited = 0;
	static uuid_node_t saved_node;
	char *seed = malloc(16);
	FILE *fp;

	if(!inited){
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
        inited = 1;
    }
	free(seed);

    *node = saved_node;
}

void get_system_time(uuid_time_t *uuid_time){
#ifdef __WIN32___
	ULARGE_INTEGER time;

    GetSystemTimeAsFileTime((FILETIME *)&time);
    time.QuadPart +=

        (unsigned __int64) (1000*1000*10)       // seconds
        * (unsigned __int64) (60 * 60 * 24)       // days
        * (unsigned __int64) (17+30+31+365*18+5); // # of days
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
int uuid_create(cuuid_t *uuid){
	uuid_time_t timestamp;
	unsigned short clockseq;
	uuid_node_t node;

	get_current_time(&timestamp);
	get_mem_seed(&node);
	clockseq = true_random();

	printf("node0 %x\n", node.nodeID[0]);
	printf("node1 %x\n", node.nodeID[1]);
	printf("node2 %x\n", node.nodeID[2]);
	printf("node3 %x\n", node.nodeID[3]);
	printf("node4 %x\n", node.nodeID[4]);
	printf("node5 %x\n", node.nodeID[5]);
	format_uuid_v1(uuid, clockseq, timestamp, node);
	return 1;
}

/* format_uuid_v1 -- make a UUID from the timestamp, clockseq, and node ID */
void format_uuid_v1(cuuid_t* uuid, unsigned short clock_seq, uuid_time_t timestamp, uuid_node_t node){
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

        /* if clock reading changed since last UUID generated, */
        if (time_last != time_now){
            /* reset count of uuids gen'd with this clock reading */
            uuids_this_tick = 0;
            time_last = time_now;
            break;
        }
        if (uuids_this_tick < UUIDS_PER_TICK){
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
	static int inited = 0;
	uuid_time_t time_now;

	if(!inited){
		get_system_time(&time_now);
		time_now = time_now / UUIDS_PER_TICK;
		srand((unsigned int)(((time_now >> 32) ^ time_now) & 0xffffffff));
		inited = 1;
	}

	return (rand()+get_tick_count());
}

/* puid -- print a UUID */
void puid(cuuid_t u){
	printf("{%.8x-", (unsigned int)u.time_low);
	printf("%.4x-", u.time_mid);
    printf("%.4x-", u.time_hi_and_version);
    printf("%x", u.clock_seq_hi_and_reserved);
    printf("%.2x-", u.clock_seq_low);

    printf("%.2x", u.node[0]); // Class
    printf("%.2x!", u.node[1]); // Category
    printf("%.2x", u.node[2]);
    printf("%.2x", u.node[3]);
    printf("%.2x", u.node[4]); // Random note
    printf("%.2x", u.node[5]); // Serie

    printf("}\n");
}

void twait(int ms){
#ifdef __WIN32__
	Sleep(ms);
#else
	usleep(ms * 1000);
#endif
}

int main(int argc, char **argv){
	cuuid_t u;
	int n;

	for(n = 0; n < 2; n++){
		uuid_create(&u);
		puid(u);
		twait(5);
	}

	printf("Generated QUID # %d\n", n);
	printf("-------------------\n");
	printf("With flags:\n");

	unsigned char data = 0xc5;
	unsigned char cat = 0x9;

    if(data & (1<<0)){
        printf("  PUBLIC\n");
    }else{
        printf("  PRIVATE\n");
    }
    if(data & (1<<2)){
        printf("  MASTER\n");
    }else{
        printf("  SLAVE\n");
    }
    if(data & (1<<4)){
        printf("  SIGNED\n");
    }else{
        printf("  UNSIGNED\n");
    }
    if(data & (1<<5)){
        printf("  TAGGED\n");
    }else{
        printf("  UNTAGGED\n");
    }

    switch(cat)
    {
    case 2:
        printf("COMMON IDENTIFIER\n");
        break;
    case 7:
        printf("INDEX IDENTIFIER\n");
        break;
    case 9:
        printf("SYSTEM IDENTIFIER\n");
        break;
    case 12:
        printf("COMMON INFO\n");
        break;
    case 13:
        printf("COMMON WARNING\n");
        break;
    case 14:
        printf("COMMON ERROR\n");
        break;
    case 22:
        printf("SYSTEM INFO\n");
        break;
    case 23:
        printf("SYSTEM WARNING\n");
        break;
    case 24:
        printf("SYSTEM ERROR\n");
        break;
    default:
        printf("UNKNOWN\n");
        break;
    }

	printf("-------------------\n");

	return EXIT_SUCCESS;
}
