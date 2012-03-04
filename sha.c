/* "sha.c" */ 

#include "sha.h" 

static unsigned long A, B, C, D, E; 

#define K1 0x5a827999 
#define K2 0x6ed9eba1 
#define K3 0x8f1bbcdc 
#define K4 0xca62c1d6 

#define S(a,n) ((a << n) | (a >> (32-n))) 

static 
unsigned char SHA_IV[20] = { 0x67, 0x45, 0x23, 0x01, 0xef, 0xcd, 0xab, 0x89, 
    0x98, 0xba, 0xdc, 0xfe, 0x10, 0x32, 0x54, 0x76, 
    0xc3, 0xd2, 0xe1, 0xf0 }; 

/* SHA ft(B,C,D) + Kt */ 

static unsigned long ftk(int t) 
{ 
    if (t < 20) 
        return( ((B & C) | (~B & D)) + K1 ); 
    else if (t < 40) 
        return( (B ^ C ^ D) + K2 ); 
    else if (t < 60) 
        return( ((B & C) | (B & D) | (C & D)) + K3 ); 
    else 
        return( (B ^ C ^ D) + K4 ); 
} 

/* the 80 rounds of SHA */ 

    static 
void shaHash(SHA_INFO *sha_info) 
{ 
    unsigned long t, A0, B0, C0, D0, E0, W[16]; 
    int i, s; 

    /* set the temporary digest values from the current digest, 
       using shifts to ensure machine-independence */ 

    A = (unsigned long)sha_info->digest[0] << 24; 
    A += (unsigned long)sha_info->digest[1] << 16; 
    A += (unsigned long)sha_info->digest[2] << 8; 
    A += (unsigned long)sha_info->digest[3]; 
    B = (unsigned long)sha_info->digest[4] << 24; 
    B += (unsigned long)sha_info->digest[5] << 16; 
    B += (unsigned long)sha_info->digest[6] << 8; 
    B += (unsigned long)sha_info->digest[7]; 
    C = (unsigned long)sha_info->digest[8] << 24; 
    C += (unsigned long)sha_info->digest[9] << 16; 
    C += (unsigned long)sha_info->digest[10] << 8; 
    C += (unsigned long)sha_info->digest[11]; 
    D = (unsigned long)sha_info->digest[12] << 24; 
    D += (unsigned long)sha_info->digest[13] << 16; 
    D += (unsigned long)sha_info->digest[14] << 8; 
    D += (unsigned long)sha_info->digest[15]; 
    E = (unsigned long)sha_info->digest[16] << 24; 
    E += (unsigned long)sha_info->digest[17] << 16; 
    E += (unsigned long)sha_info->digest[18] << 8; 
    E += (unsigned long)sha_info->digest[19]; 

    /* save A-E */ 

    A0 = A; 
    B0 = B; 
    C0 = C; 
    D0 = D; 
    E0 = E; 

    /* move the data into the first 16 words of W */ 

    for (i = 0; i < 16; i++) 
        W[i] = sha_info->data[i]; 

    /* perform the 80 rounds, using the "alternate method" in which 
       the later values of W are computed in place */ 

    for (i = 0; i < 80; i++) 
    { 
        s = i & 0x0f; 

        if (i >= 16) 
        { 
            t = W[(i-3) & 0x0f] ^ W[(i-8) & 0x0f] ^ 
                W[(i-14)&0x0f] ^ W[s]; 
            W[s] = S(t,1); 
        } 

        t = S(A,5) + ftk(i) + E + W[s]; 
        E = D; 
        D = C; 
        C = S(B,30); 
        B = A; 
        A = t; 
    } 

    /* add in the original values of A-E */ 

    A += A0; 
    B += B0; 
    C += C0; 
    D += D0; 
    E += E0; 

    /* save resulting digest, again using shifts to ensure 
       machine independence */ 

    sha_info->digest[0] = (unsigned char)(A >> 24); 
    sha_info->digest[1] = (unsigned char)((A >> 16) & 0xff); 
    sha_info->digest[2] = (unsigned char)((A >> 8) & 0xff); 
    sha_info->digest[3] = (unsigned char)(A & 0xff); 
    sha_info->digest[4] = (unsigned char)(B >> 24); 
    sha_info->digest[5] = (unsigned char)((B >> 16) & 0xff); 
    sha_info->digest[6] = (unsigned char)((B >> 8) & 0xff); 
    sha_info->digest[7] = (unsigned char)(B & 0xff); 
    sha_info->digest[8] = (unsigned char)(C >> 24); 
    sha_info->digest[9] = (unsigned char)((C >> 16) & 0xff); 
    sha_info->digest[10] = (unsigned char)((C >> 8) & 0xff); 
    sha_info->digest[11] = (unsigned char)(C & 0xff); 
    sha_info->digest[12] = (unsigned char)(D >> 24); 
    sha_info->digest[13] = (unsigned char)((D >> 16) & 0xff); 
    sha_info->digest[14] = (unsigned char)((D >> 8) & 0xff); 
    sha_info->digest[15] = (unsigned char)(D & 0xff); 
    sha_info->digest[16] = (unsigned char)(E >> 24); 
    sha_info->digest[17] = (unsigned char)((E >> 16) & 0xff); 
    sha_info->digest[18] = (unsigned char)((E >> 8) & 0xff); 
    sha_info->digest[19] = (unsigned char)(E & 0xff); 

    /* clear the data so that further updates can be added in */ 

    for (i = 0; i < 16; i++) 
        sha_info->data[i] = 0; 
} 

/* initialize sha_info */ 

void shaInitial(SHA_INFO *sha_info) 
{ 
    int i; 

    /* set digest to its initial value. Done one char at a time 
       to ensure machine independence. */ 

    for (i = 0; i < 20; i++) 
        sha_info->digest[i] = SHA_IV[i]; 

    /* clear data so that updates can be added in */ 

    for (i = 0; i < 16; i++) 
        sha_info->data[i] = 0; 

    /* set bit count to zero */ 

    sha_info->count[0] = sha_info->count[1] = 0; 
} 

/* update the digest using additional message data */ 

void shaUpdate(SHA_INFO *sha_info, 
        unsigned char *buffer, 
        unsigned long offset, 
        unsigned long count) 
{ 
    unsigned long data_count, t, mask_size; 
    unsigned char *bptr, c, last, mask; 

    /* enter the message data into the data buffer. When the buffer 
       is full (512 bits entered, update the digest and clear the 
       data buffer for the next update. */ 

    data_count = sha_info->count[1]%512; 
    bptr = buffer + (offset/8); 

    /* first fill the current octet of the buffer, so that 
       the bit offset into the buffer is a multiple of 8 */ 
    last = *bptr++; 
    if (data_count%8) 
    { 
        /* get a full byte from the buffer */ 
        c = last; 
        last = *bptr++; 
        if (offset%8) 
        { 
            c <<= (offset%8); 
            c += last >> (8 - (offset%8)); 
        } 

        /* set mask to fill the remaining bits of the octet */ 
        mask_size = 8 - (data_count%8); 
        mask = 0xff << (data_count%8); 

        /* adjust for short count */ 
        if (count < mask_size) 
        { 
            mask <<= (mask_size-count); 
            mask_size = count; 
        } 

        /* store the bits */ 
        c = (c & mask) >> (data_count%8); 
        sha_info->data[data_count/32] += (unsigned long)c << 8*(3 - 
                ((data_count%32)/8)); 

        /* update count */ 
        t = sha_info->count[1]; 
        sha_info->count[1] += mask_size; 
        if (sha_info->count[1] < t) 
            sha_info->count[0]++; 

        /* if the data buffer is full, update the digest */ 
        data_count += mask_size; 
        if (data_count == 512) 
        { 
            shaHash(sha_info); 
            data_count = 0; 
        } 

        /* start over with updated offset and count */ 
        offset += mask_size; 
        count -= mask_size; 
        bptr = buffer + (offset/8); 
        last = *bptr++; 
    } 
    while (count != 0) 
    { 
        /* get the next full octet from the buffer */ 
        c = last; 
        last = *bptr++; 
        if (offset%8) 
        { 
            c <<= (offset%8); 
            c += last >> (8 - (offset%8)); 
        } 

        /* set mask to a full octet */ 
        mask_size = 8; 
        mask = 0xff; 

        /* adjust for short count */ 
        if (count < mask_size) 
        { 
            mask <<= (mask_size-count); 
            mask_size = count; 
        } 

        /* store the bits */ 
        c &= mask; 
        sha_info->data[data_count/32] += (unsigned long)c << 8*(3 - 
                ((data_count%32)/8)); 

        /* update count */ 
        t = sha_info->count[1]; 
        sha_info->count[1] += mask_size; 
        if (sha_info->count[1] < t) 
            sha_info->count[0]++; 

        /* if the data buffer is full, update the digest */ 
        data_count += mask_size; 
        if (data_count == 512) 
        { 
            shaHash(sha_info); 
            data_count = 0; 
        } 

        count -= mask_size; 
    } 
} 

/* add pad bit of '1', zero fill and bit length, then update the digest */ 

void shaFinal(SHA_INFO *sha_info) 
{ 
    /* add the pad bit of '1' */ 

    sha_info->data[(sha_info->count[1]%512)/32] += 
        1L << (31 - (sha_info->count[1]%32)); 

    /* if the data buffer is full, update the digest */ 

    if ((sha_info->count[1]%512) == 511) 
        shaHash(sha_info); 

    /* if there isn't room for 64 bits of bit length, leave the 
       buffer zero filled to the end, update the digest and clear 
       the buffer. */ 

    if ((sha_info->count[1]%512) > (512-65)) 
        shaHash(sha_info); 

    /* put in the bit length */ 

    sha_info->data[14] = sha_info->count[0]; 
    sha_info->data[15] = sha_info->count[1]; 

    /* update the digest */ 

    shaHash(sha_info); 
} 
