#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "lib/hiredis/hiredis.h"

typedef unsigned char byte;

/* Global junk */
char *testStrings[] = {
    "aaaaa",
    "aaaab",
    "bbbbb",
    "cat",
    "dog",
    "hello",
    "laura",
    "scout"
};

// swap using char pointers
unsigned long long  swap(double d)
{
    unsigned long long a;
    unsigned char *dst = (unsigned char *)&a;
    unsigned char *src = (unsigned char *)&d;

    dst[0] = src[7];
    dst[1] = src[6];
    dst[2] = src[5];
    dst[3] = src[4];
    dst[4] = src[3];
    dst[5] = src[2];
    dst[6] = src[1];
    dst[7] = src[0];

    return a;
}

double atof_raw(const char*);
double atof_raw_swap(const char*);

int main(int argc, char *argv[]) {
    unsigned int j;
    redisContext *c;
    redisReply *reply;

    struct timeval timeout = { 1, 500000 }; // 1.5 seconds
    c = redisConnectWithTimeout((char*)"127.0.0.2", 6379, timeout);
    if (c->err) {
        printf("Connection error: %s\n", c->errstr);
        exit(1);
    }


    for (int i=0; i < sizeof(testStrings)/sizeof(char*); i++) {
        double score = atof_raw(testStrings[i]);
        reply = redisCommand(c, "ZADD %s %.20e %s", "ascii|test", score, testStrings[i]);
        printf("result: %s\n", reply->str);
        freeReplyObject(reply);
    }
    
    return 0;
}

/* 
    double atof_raw(const char *)
    
    converts an ASCII string to a well ordered floating point representation
*/
const int NULL_CHAR = 0x00;
const int ASCII_WIDTH = 7;
typedef unsigned long ulong_t;
double atof_raw(const char* s) {
    size_t sLength = strlen(s);

    if (sLength > 9) {
        /* too big to fit in a double naively */
        printf("%s is too large to be packed in a double\n", s);
        exit(1);
    }

    double out = 0.0;
    byte *dest = (byte *)&out;
    memcpy(dest, s, sLength);
    out = swap(out);
    return out;
}
