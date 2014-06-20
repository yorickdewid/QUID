#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "main.h"

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

	for(n = 0; n < 8000000; n++){
		uuid_create(&u);
		puid(u);
//		twait(5);
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
