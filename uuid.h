typedef struct {
    unsigned long   time_low;
    unsigned short  time_mid;
    unsigned short  time_hi_and_version;
    unsigned char   clock_seq_hi_and_reserved;
    unsigned char   clock_seq_low;
    unsigned char   node[6];
} cuuid_t;

typedef unsigned long long uuid_time_t;

typedef struct {
    char nodeID[6];
} uuid_node_t;
