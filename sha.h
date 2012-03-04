/* "sha.h" */ 

#ifndef SHA_H 
#define SHA_H 

/* header for SHA and related procedures */ 
#define WORD unsigned long 
#define DIGEST_LENGTH 20 
typedef struct { 
    unsigned char digest[DIGEST_LENGTH]; /* Message digest */ 
    WORD count[2]; /* count of bits */ 
    WORD data[16]; /* data buffer */ 
} 
SHA_INFO; 

void shaInitial(SHA_INFO *sha_info); 
void shaUpdate(SHA_INFO *sha_info, 
        unsigned char *buffer, 
        unsigned long offset, 
        unsigned long count); 
void shaFinal(SHA_INFO *sha_info); 

#endif 
