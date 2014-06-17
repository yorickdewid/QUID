#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#ifdef __WIN32__
#include <unistd.h>
#include <winsock2.h>
#else
#include <unistd.h>
#endif
#include "uuid.h"

#define UUIDS_PER_TICK 1024
#define EPOCH_DIFF 11644473600LL

/* various forward declarations */
static void format_uuid_v1(cuuid_t *uuid, unsigned short clockseq, uuid_time_t timestamp, uuid_node_t node);
static void get_current_time(uuid_time_t *timestamp);
static void get_ieee_node_identifier(uuid_node_t *node);
static void get_system_time(uuid_time_t *uuid_time);
static unsigned short true_random();
static int uuid_create(cuuid_t * uuid);

void get_ieee_node_identifier(uuid_node_t *node)
{
    static int inited = 0;
    static uuid_node_t saved_node;
    char seed[16];
    FILE *fp;

    //if(!inited){
        //fp = fopen("nodeid", "rb");
        //if(fp){
        //    fread(&saved_node, sizeof saved_node, 1, fp);
        //    fclose(fp);
        //}else{
            //get_random_info(seed);
            seed[0] |= 0x01;
            memcpy(&saved_node, seed, sizeof(saved_node));
            printf("memgrap %x\n", seed);
            fp = fopen("nodeid", "wb");
            if(fp){
                fwrite(&saved_node, sizeof saved_node, 1, fp);
                fclose(fp);
            }
        //}
    //    inited = 1;
    //}

    *node = saved_node;
}

void get_system_time(uuid_time_t *uuid_time)
{
#ifdef __WIN32___
    ULARGE_INTEGER time;

    /* NT keeps time in FILETIME format which is 100ns ticks since
       Jan 1, 1601. UUIDs use time in 100ns ticks since Oct 15, 1582.
       The difference is 17 Days in Oct + 30 (Nov) + 31 (Dec)
       + 18 years and 5 leap days. */
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

/* uuid_create -- generator a UUID */
int uuid_create(cuuid_t *uuid)
{
    uuid_time_t timestamp;
    unsigned short clockseq;
    uuid_node_t node;

    get_current_time(&timestamp);
    get_ieee_node_identifier(&node);
    clockseq = true_random();

    format_uuid_v1(uuid, clockseq, timestamp, node);
    return 1;
}

/* format_uuid_v1 -- make a UUID from the timestamp, clockseq, and node ID */
void format_uuid_v1(cuuid_t* uuid, unsigned short clock_seq, uuid_time_t timestamp, uuid_node_t node)
{
    /* Construct a version 1 uuid with the information we've gathered plus a few constants. */
    //printf("timestamp %016lld\t\t %016llx\n", timestamp, timestamp);

    uuid->time_low = (unsigned long)(timestamp & 0xFFFFFFFF);
    //printf("uuid->time_low %d\t\t %x\n", (int)uuid->time_low, (int)uuid->time_low);

    uuid->time_mid = (unsigned short)((timestamp >> 32) & 0xFFFF);

    //printf("uuid->time_mid %d\t\t\t %x\n", (int)uuid->time_mid, (int)uuid->time_mid);
    //printf("options \t\t\t\t %x\n", ( ( timestamp >> 40) & 0xFF) );
    //printf("options \t\t\t\t %x\n", ( ( timestamp >> 32) & 0xFF) );

    uuid->time_hi_and_version = (unsigned short)((timestamp >> 48) & 0xFFF);
    uuid->time_hi_and_version ^= 0x80;
    //printf("uuid->time_hi %x\t\t\t %x\n", (int)uuid->time_hi_and_version, (int)uuid->time_hi_and_version^0x80);

    uuid->time_hi_and_version |= 0xa000;
    //printf("uuid->time_hi_and_version %d\t\t %x\n", (int)uuid->time_hi_and_version, (int)uuid->time_hi_and_version);

    uuid->clock_seq_low = (clock_seq & 0xFF);
    //printf("uuid->clock_seq_low %d\t\t\t %x\n", (int)uuid->clock_seq_low, (int)uuid->clock_seq_low);

    uuid->clock_seq_hi_and_reserved = (clock_seq & 0x3F00) >> 8;
    //printf("uuid->clock_seq_hi %d\t\t\t %x\n", (int)uuid->clock_seq_hi_and_reserved, (int)uuid->clock_seq_hi_and_reserved);

    uuid->clock_seq_hi_and_reserved |= 0x80;
    //printf("uuid->clock_seq_hi_and_reserved %d\t %x\n", (int)uuid->clock_seq_hi_and_reserved, (int)uuid->clock_seq_hi_and_reserved);

    memcpy(&uuid->node, &node, sizeof(uuid->node));
    uuid->node[0] = 0xec; // Class
    uuid->node[1] = 0x16; // Category
    uuid->node[5] = (true_random() & 0xFF);
    //printf("uuid->node %d\t\t\t %x\n\n", (int)uuid->node, (int)uuid->node);
}

void get_current_time(uuid_time_t *timestamp)
{
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

double get_tick_count()
{
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

static unsigned short true_random()
{
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
void puid(cuuid_t u)
{
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

void twait(int ms)
{
#ifdef __WIN32__
    Sleep(ms);
#else
    usleep(ms * 1000);
#endif
}

int main(int argc, char **argv)
{
    cuuid_t u;
    int n;

    for(n = 0; n < 10; n++){
        uuid_create(&u);
        puid(u);
        twait(100);
    }

    printf("Generated QUID # %d\nWith options:\n", n);

    unsigned char data = 0xc5;
    unsigned char cat = 0x9;

    if(data & (1<<0)){
        printf("PUBLIC\n");
    }else{
        printf("PRIVATE\n");
    }
    if(data & (1<<2)){
        printf("MASTER\n");
    }else{
        printf("SLAVE\n");
    }
    if(data & (1<<4)){
        printf("SIGNED\n");
    }else{
        printf("UNSIGNED\n");
    }
    if(data & (1<<5)){
        printf("TAGGED\n");
    }else{
        printf("UNTAGGED\n");
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

    return EXIT_SUCCESS;
}
