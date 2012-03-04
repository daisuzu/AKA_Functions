/* aka.c: sha based function for aka */ 

#include <string.h> 
#include "aka.h" 

static uchar counter[8]={0}; 

static uchar G[20] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x2d}; 
static uchar A[20] = { 0x9d, 0xe9, 0xc9, 0xc8, 0xef, 0xd5, 0x78, 0x11, 
    0x48, 0x23, 0x14, 0x01, 0x90, 0x1f, 0x2d, 0x49, 
    0x3f, 0x4c, 0x63, 0x65}; 
static uchar B[20] = { 0x75, 0xef, 0xd1, 0x5c, 0x4b, 0x8f, 0x8f, 0x51, 
    0x4e, 0xf3, 0xbc, 0xc3, 0x79, 0x4a, 0x76, 0x5e, 
    0x7e, 0xec, 0x45, 0xe0}; 
static 
void modred(uchar *z,int shift,uchar *base); 

/* This function performs the operation of (A*X+B) mod 2^160+2^5+2^3+2^2+1 
 * 
 * 
 */ 

void whiten(uchar xx[]) 
{ 
    uchar z[40]; 
    int i, j; 

    /* calculate A * X in polynomial form */ 
    for (i=0;i<40;i++) 
        z[i]=0; 

    for (i=0;i<20;i++) 
    { 
        for (j=0;j<8;j++) 
        { 
            if ((xx[i]<<j) & 0x80) 
                modred(z,159-(i*8+j),A); /* z^=A<<(159-(i*8+j)) */ 
        } 
    } 


    /* AX MOD G done as modular reduction for bit 160 to 319 */ 
    for (i=0;i<20;i++) 
    { 
        for (j=0;j<8;j++) 
        { 
            if ((z[i]<<j)&0x80) 
                modred(z,159-(i*8+j),G); 
        } 
    } 

    /* add B and copy back result */ 
    for (i = 0; i < 20; i++) 
        xx[i] = z[i+20] ^ B[i]; 
} 

/* This function perform the operation of shifting 320 bits and XOR. 
 * 
 * 
 */ 

    static 
void modred(uchar *z,int shift,uchar *base) 
{ 
    int byteshift, bitshift,i; 
    uchar q[21],yn,yn1; 

    for (i=0;i<20;i++) 
        q[i] = base[i]; 
    q[20] = 0; 

    /* we divide into byte shifting and bit shifting */ 
    byteshift = shift / 8; 
    bitshift = shift % 8; 

    /* do bit shifting */ 
    if (bitshift != 0) 
    { 
        yn = 0; 
        for (i = 0; i <= 20; i++) 
        { 
            yn1 = yn; 
            yn = q[i]; 
            q[i] >>= 8-bitshift; 
            q[i] |= yn1 << bitshift; 
        } 
        /* shift one more byte, since bits have effectively been 
           shifted into the next byte upward */ 
        byteshift++; 
    } 

    /* z ^= q and send back result in z */ 
    for (i = 0; i < 20; i++) 
        z[i+20-byteshift] ^= q[i]; 
    if (bitshift != 0) 
        z[40-byteshift] ^= q[20]; 
} 

/* This function performs generation of 64-bit pseudo random number RAND. 
 * 
 * 
 */ 

    void 
f0(uchar seed[],uchar fi,uchar Fmk[],uchar buff[]) 
{ 
    SHA_INFO sha_info; 
    uchar buf[64]; 
    uchar t; 
    int i; 

    shaInitial(&sha_info); 
    for (i = 0; i < L_KEY; i++) 
        sha_info.digest[i] ^= seed[i]; 

    for (i = 0; i < 64; i++) 
        buf[i] = 0x5c; 

    for (i = 0; i < 8; i++) 
    { 
        buf[i] ^= counter[i]; 
        buf[i+16] ^= counter[i]; 
        buf[i+32] ^= counter[i]; 
        buf[i+48] ^= counter[i]; 
    } 

    buf[11] ^= fi; 

    for (i = 0; i < 4; i++) 
        buf[i+12] ^= Fmk[i]; 

    shaUpdate(&sha_info,buf,0,512); 

    /* perform (AX+B)mod G */ 
    whiten(sha_info.digest); 

    /* get 8 bytes or 64 bits */ 
    for (i=0;i<8;i++) 
        buff[i] = sha_info.digest[i]; 

    /* increment counter */ 
    for (i = 7; i >= 0; i--) 
    { 
        t = counter[i]; 
        counter[i]++; 
        if (counter[i] > t) 
            break; 
    } 
} 

/* This function performs generation of authentication signature MACA. 
 * 
 * 
 */ 

void 
f1(uchar K[],uchar fi,uchar *RAND,uchar Fmk[],uchar SQN[],uchar AMF[],uchar 
        MACA[]) 
{ 
    SHA_INFO sha_info; 
    uchar buf[64]; 
    int i; 

    /* NOTE: the following initialization of the sha_info struct can be 
       performed 
       once when K is provisioned, and the results copied into sha_info at the 
       start of this function. */ 
    shaInitial(&sha_info); 
    for (i = 0; i < L_KEY; i++) 
        sha_info.digest[i] ^= K[i]; 

    for (i = 0; i < 64; i++) 
        buf[i] = 0x5c; 

    for (i = 0; i < 4; i++) 
        buf[i+12] ^= Fmk[i]; 
    for (i = 0; i < 16; i++) 
        buf[i+16] ^= RAND[i]; 
    for (i = 0; i < 6; i++) 
        buf[i+34] ^= SQN[i]; 
    for (i = 0; i < 2; i++) 
        buf[i+42] ^= AMF[i]; 

    buf[11] ^= fi; 

    shaUpdate(&sha_info,buf,0,512); 

    /* perform (AX+B)mod G */ 
    whiten(sha_info.digest); 

    for (i=0;i<L_MACA;i++) 
        MACA[i] = sha_info.digest[i]; 
} 

/* This function performs generation of resync authentication signature MACS. 
 * 
 * 
 */ 

void 
f1star(uchar K[],uchar fi,uchar *RAND,uchar Fmk[],uchar SQN[],uchar AMF[],uchar 
        MACS[]) 
{ 
    SHA_INFO sha_info; 
    uchar buf[64]; 
    int i; 

    /* NOTE: the following initialization of the sha_info struct can be 
       performed 
       once when K is provisioned, and the results copied into sha_info at the 
       start of this function. */ 
    shaInitial(&sha_info); 
    for (i = 0; i < L_KEY; i++) 
        sha_info.digest[i] ^= K[i]; 

    for (i = 0; i < 64; i++) 
        buf[i] = 0x5c; 

    for (i = 0; i < 4; i++) 
        buf[i+12] ^= Fmk[i]; 
    for (i = 0; i < 16; i++) 
        buf[i+16] ^= RAND[i]; 
    for (i = 0; i < 6; i++) 
        buf[i+34] ^= SQN[i]; 
    for (i = 0; i < 2; i++) 
        buf[i+42] ^= AMF[i]; 

    buf[11] ^= fi; 

    shaUpdate(&sha_info,buf,0,512); 

    /* perform (AX+B)mod G */ 
    whiten(sha_info.digest); 

    for (i=0;i<L_MACS;i++) 
        MACS[i] = sha_info.digest[i]; 
} 

/* This function performs generation of user response RES. 
 * 
 * 
 */ 

    void 
f2(uchar K[],uchar fi,uchar *RAND,uchar Fmk[],uchar RES[],int l_res) 
{ 
    SHA_INFO sha_info; 
    uchar j,buf[64]; 
    int i; 

    if (l_res < 1) 
        return; 
    if (l_res > 16) 
        l_res = 16; 

    for (j = 0; j < 2; j++) 
    { 
        /* NOTE: the following initialization of the sha_info struct can be 
           performed 
           once when K is provisioned, and the results copied into sha_info at 
           the 
           start of this loop. */ 
        shaInitial(&sha_info); 
        for (i = 0; i < L_KEY; i++) 
            sha_info.digest[i] ^= K[i]; 

        for (i = 0; i < 64; i++) 
            buf[i] = 0x5c; 

        for (i = 0; i < 4; i++) 
            buf[i+12] ^= Fmk[i]; 
        for (i = 0; i < 16; i++) 
            buf[i+24] ^= RAND[i]; 

        buf[3] ^= j; 
        buf[11] ^= fi; 
        buf[19] ^= j; 
        buf[35] ^= j; 
        buf[51] ^= j; 

        shaUpdate(&sha_info,buf,0,512); 

        whiten(sha_info.digest); 
        for (i=0;i<8;i++) 
        { 
            RES[8*j+i] = sha_info.digest[i]; 
            if (--l_res == 0) 
                return; 
        } 
    } 
} 

/* This function performs generation of cipher key CK. 
 * 
 * 
 */ 

    void 
f3(uchar K[],uchar fi,uchar *RAND,uchar Fmk[],uchar *CK) 
{ 
    SHA_INFO sha_info; 
    uchar j,buf[64]; 
    int i; 

    for (j = 0; j < 2; j++) 
    { 
        /* NOTE: the following initialization of the sha_info struct can be 
           performed 
           once when K is provisioned, and the results copied into sha_info at 
           the 
           start of this loop. */ 
        shaInitial(&sha_info); 
        for (i = 0; i < L_KEY; i++) 
            sha_info.digest[i] ^= K[i]; 

        for (i = 0; i < 64; i++) 
            buf[i] = 0x5c; 

        for (i = 0; i < 4; i++) 
            buf[i+12] ^= Fmk[i]; 
        for (i = 0; i < 16; i++) 
            buf[i+24] ^= RAND[i]; 

        buf[3] ^= j; 
        buf[11] ^= fi; 
        buf[19] ^= j; 
        buf[35] ^= j; 
        buf[51] ^= j; 

        shaUpdate(&sha_info,buf,0,512); 

        whiten(sha_info.digest); 
        for (i=0;i<8;i++) 
            CK[8*j+i] = sha_info.digest[i]; 
    } 
} 

/* This function performs generation of integrity key IK. 
 * 
 * 
 */ 

    void 
f4(uchar K[],uchar fi,uchar *RAND,uchar Fmk[],uchar *IK) 
{ 
    SHA_INFO sha_info; 
    uchar j,buf[64]; 
    int i; 

    for (j = 0; j < 2; j++) 
    { 
        /* NOTE: the following initialization of the sha_info struct can be 
           performed 
           once when K is provisioned, and the results copied into sha_info at 
           the 
           start of this loop. */ 
        shaInitial(&sha_info); 
        for (i = 0; i < L_KEY; i++) 
            sha_info.digest[i] ^= K[i]; 

        for (i = 0; i < 64; i++) 
            buf[i] = 0x5c; 

        for (i = 0; i < 4; i++) 
            buf[i+12] ^= Fmk[i]; 
        for (i = 0; i < 16; i++) 
            buf[i+24] ^= RAND[i]; 

        buf[3] ^= j; 
        buf[11] ^= fi; 
        buf[19] ^= j; 
        buf[35] ^= j; 
        buf[51] ^= j; 

        shaUpdate(&sha_info,buf,0,512); 

        whiten(sha_info.digest); 
        for (i=0;i<8;i++) 
            IK[8*j+i] = sha_info.digest[i]; 
    } 
} 

/* This function performs generation of anonymity key AK. 
 * 
 * 
 */ 

    void 
f5(uchar K[],uchar fi,uchar *RAND,uchar Fmk[],uchar AK[]) 
{ 
    SHA_INFO sha_info; 
    uchar buf[64]; 
    int i; 

    /* NOTE: the following initialization of the sha_info struct can be 
       performed 
       once when K is provisioned, and the results copied into sha_info at the 
       start of this function. */ 
    shaInitial(&sha_info); 
    for (i = 0; i < L_KEY; i++) 
        sha_info.digest[i] ^= K[i]; 

    for (i = 0; i < 64; i++) 
        buf[i] = 0x5c; 

    for (i = 0; i < 4; i++) 
        buf[i+12] ^= Fmk[i]; 
    for (i = 0; i < 16; i++) 
        buf[i+16] ^= RAND[i]; 

    buf[11] ^= fi; 

    shaUpdate(&sha_info,buf,0,512); 

    /* perform (AX+B)mod G */ 
    whiten(sha_info.digest); 

    for (i=0;i<L_AK;i++) 
        AK[i] = sha_info.digest[i]; 
} 

/* This function performs generation of anonymity key AK for 
 * resynchronization. 
 * 
 * 
 */ 

    void 
f5star(uchar K[],uchar fi,uchar *RAND,uchar Fmk[],uchar AKS[]) 
{ 
    SHA_INFO sha_info; 
    uchar buf[64]; 
    int i; 

    /* NOTE: the following initialization of the sha_info struct can be 
       performed 
       once when K is provisioned, and the results copied into sha_info at the 
       start of this function. */ 
    shaInitial(&sha_info); 
    for (i = 0; i < L_KEY; i++) 
        sha_info.digest[i] ^= K[i]; 

    for (i = 0; i < 64; i++) 
        buf[i] = 0x5c; 

    for (i = 0; i < 4; i++) 
        buf[i+12] ^= Fmk[i]; 
    for (i = 0; i < 16; i++) 
        buf[i+16] ^= RAND[i]; 

    buf[11] ^= fi; 

    shaUpdate(&sha_info,buf,0,512); 

    /* perform (AX+B)mod G */ 
    whiten(sha_info.digest); 

    for (i=0;i<L_AKS;i++) 
        AKS[i] = sha_info.digest[i]; 
} 

/* This function performs generation of UMAC key UAK. 
 * 
 * 
 */ 

    void 
f11(uchar K[],uchar fi,uchar *RAND,uchar Fmk[],uchar *UAK) 
{ 
    SHA_INFO sha_info; 
    uchar i, buf[64]; 
    int j; 

    for(i=0;i<2;i++) 
    { 
        /* NOTE: the following initialization of the sha_info struct can be 
           performed 
           once when K is provisioned, and the results copied into sha_info at 
           the 
           start of this loop. */ 
        shaInitial(&sha_info); 
        for (j = 0; j < L_KEY; j++) 
            sha_info.digest[j] ^= K[j]; 

        for (j = 0; j < 64; j++) 
            buf[j] = 0x5c; 

        for (j = 0; j < 4; j++) 
            buf[j+12] ^= Fmk[j]; 
        for (j = 0; j < 16; j++) 
            buf[j+24] ^= RAND[j]; 

        buf[3] ^= i; 
        buf[11] ^= fi; 
        buf[19] ^= i; 
        buf[35] ^= i; 
        buf[51] ^= i; 

        shaUpdate(&sha_info,buf,0,512); 

        whiten(sha_info.digest); 

        for (j=0;j<8;j++) 
            UAK[8*i+j] = sha_info.digest[j]; 
    } 
} 
