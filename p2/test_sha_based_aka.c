/* test_sha_based_aka.c */ 

#include <stdio.h> 
#include "aka.h" 

unsigned char *msg_data = 
"abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq"; 

void pause(void) 
{ 
    printf("Press any key to continue\n"); 
    getchar(); 
} 

void main() 
{ 
    uchar K[]={0xad,0x1b,0x5a,0x15,0x9b,0xe8,0x6b,0x2c, 
        0xa6,0x6c,0x7a,0xe4,0x0b,0xba,0x9b,0x9d}; 

    uchar seed[]={0xb0,0xab,0xb9,0x9d,0x6a,0xc6,0xa7,0x4e, 
        0xb9,0x8e,0xb6,0xc2,0xda,0xb1,0xa5,0x51}; 

    uchar Fmk[L_FMK] = { 'A', 'H', 'A', 'G' }; 
    uchar RAND[L_RAND]; 

    uchar CK[L_CK]; 
    uchar IK[L_IK]; 
    uchar UAK[L_UAK]; 

    uchar MACA[L_MACA]; 
    uchar MACS[L_MACS]; 
    uchar RES[L_RES]; 
    uchar AK[L_AK]; 
    uchar AKS[L_AKS]; 

    uchar SQN[L_SQN]={0x00,0x00,0x00,0x00,0x00,0x01}; 
    uchar fi0,fi1,fi1star,fi2,fi3,fi4,fi5,fi5star,fi11; 

    uchar buff1[L_RAND/2],buff2[L_RAND/2]; 

    uchar AMF[2]; 

    int i; 

    fi0=0x41; 
    fi1=0x42; 
    fi1star=0x43; 
    fi2=0x44; 
    fi3=0x45; 
    fi4=0x46; 
    fi5=0x47; 
    fi5star=0x48; 
    fi11 = 0x50; 

    printf("test vector for f0:\n"); 
    printf("input section\n"); 
    printf("seed is: "); 
    for(i=0;i<L_KEY;i++) 
        printf("%02x ",seed[i]); 
    printf("\n"); 
    printf("fi0 is: %02x\n",fi0); 
    printf("Fmk is: "); 
    for (i=0;i<L_FMK;i++) 
        printf("%02x ",Fmk[i]); 
    printf("\n"); 

    printf("\n"); 
    f0(seed,fi0,Fmk,buff1); 
    f0(seed,fi0,Fmk,buff2); 
    printf("output section\n"); 
    printf("f0 RAND: "); 
    for (i=0;i<L_RAND/2;i++) 
        printf("%02x ",buff1[i]); 
    for (i=0;i<L_RAND/2;i++) 
        printf("%02x ",buff2[i]); 
    printf("\n"); 

    pause(); 

    /* reuse RAND generated for the subsequent function calls*/ 
    for (i=0;i<L_RAND/2;i++) 
        RAND[i] = buff1[i]; 
    for (i=0;i<L_RAND/2;i++) 
        RAND[i+L_RAND/2] = buff2[i]; 

    AMF[0]=0x00; 
    AMF[1]=0x01; 

    printf("\n"); 
    printf("\n"); 
    printf("test vector for f1:\n"); 
    printf("input section\n"); 
    printf("K is: "); 
    for(i=0;i<L_KEY;i++) 
        printf("%02x ",K[i]); 
    printf("\n"); 
    printf("fi1 is: %02x\n",fi1); 
    printf("RAND is: "); 
    for(i=0;i<L_RAND;i++) 
        printf("%02x ",RAND[i]); 
    printf("\n"); 
    printf("Fmk is: "); 
    for(i=0;i<L_FMK;i++) 
        printf("%02x ",Fmk[i]); 
    printf("\n"); 
    printf("SQN is: "); 
    for(i=0;i<L_SQN;i++) 
        printf("%02x ",SQN[i]); 
    printf("\n"); 
    printf("AMF is: %02x %02x\n",AMF[0],AMF[1]); 

    f1(K,fi1,RAND,Fmk,SQN,AMF,MACA); 
    printf("\n"); 
    printf("output section\n"); 
    printf("f1 MACA: "); 
    for (i=0;i<L_MACA;i++) 
        printf("%02x ",MACA[i]); 
    printf("\n"); 

    pause(); 

    printf("\n"); 
    printf("\n"); 
    printf("test vector for f1*:\n"); 
    printf("input section\n"); 
    printf("K is: "); 
    for(i=0;i<L_KEY;i++) 
        printf("%02x ",K[i]); 
    printf("\n"); 
    printf("fi1star is: %02x\n",fi1star); 
    printf("RAND is: "); 
    for(i=0;i<L_RAND;i++) 
        printf("%02x ",RAND[i]); 
    printf("\n"); 
    printf("Fmk is: "); 
    for(i=0;i<L_FMK;i++) 
        printf("%02x ",Fmk[i]); 
    printf("\n"); 
    printf("SQN is: "); 
    for(i=0;i<L_SQN;i++) 
        printf("%02x ",SQN[i]); 
    printf("\n"); 
    printf("AMF is: %02x %02x\n",AMF[0],AMF[1]); 

    printf("\n"); 
    f1star(K,fi1star,RAND,Fmk,SQN,AMF,MACS); 
    printf("output section\n"); 
    printf("f1* MACS: "); 
    for(i=0;i<L_MACS;i++) 
        printf("%02x ",MACS[i]); 
    for(i=0;i<L_SQN;i++) 
        SQN[i]=0x00; 
    AMF[0]=AMF[1]=0; 
    printf("\n"); 

    pause(); 

    printf("\n"); 
    printf("\n"); 
    printf("test vector for f2:\n"); 
    printf("input section\n"); 
    printf("K is: "); 
    for(i=0;i<L_KEY;i++) 
        printf("%02x ",K[i]); 
    printf("\n"); 
    printf("fi2 is: %02x\n",fi2); 
    printf("RAND is: "); 
    for(i=0;i<L_RAND;i++) 
        printf("%02x ",RAND[i]); 
    printf("\n"); 
    printf("Fmk is: "); 
    for(i=0;i<L_FMK;i++) 
        printf("%02x ",Fmk[i]); 
    printf("\n"); 
    printf("\n"); 
    f2(K,fi2,RAND,Fmk,RES,L_RES); 
    printf("output section\n"); 
    printf("f2 RES: "); 
    for (i=0;i<L_RES;i++) 
        printf("%02x ",RES[i]); 
    printf("\n"); 

    pause(); 

    printf("\n"); 
    printf("\n"); 
    printf("test vector for f3:\n"); 
    printf("input section\n"); 
    printf("K is: "); 
    for(i=0;i<L_KEY;i++) 
        printf("%02x ",K[i]); 
    printf("\n"); 
    printf("fi3 is: %02x\n",fi3); 
    printf("RAND is: "); 
    for(i=0;i<L_RAND;i++) 
        printf("%02x ",RAND[i]); 
    printf("\n"); 
    printf("Fmk is: "); 
    for(i=0;i<L_FMK;i++) 
        printf("%02x ",Fmk[i]); 
    printf("\n"); 

    printf("\n"); 
    f3(K,fi3,RAND,Fmk,CK); 
    printf("output section\n"); 
    printf("f3 CK: "); 
    for (i=0;i<L_CK;i++) 
        printf("%02x ",CK[i]); 
    printf("\n"); 

    pause(); 

    printf("\n"); 
    printf("\n"); 
    printf("test vector for f4:\n"); 
    printf("input section\n"); 
    printf("K is: "); 
    for(i=0;i<L_KEY;i++) 
        printf("%02x ",K[i]); 
    printf("\n"); 
    printf("fi4 is: %02x\n",fi4); 
    printf("RAND is: "); 
    for(i=0;i<L_RAND;i++) 
        printf("%02x ",RAND[i]); 
    printf("\n"); 
    printf("Fmk is: "); 
    for(i=0;i<L_FMK;i++) 
        printf("%02x ",Fmk[i]); 
    printf("\n"); 

    printf("\n"); 
    f4(K,fi4,RAND,Fmk,IK); 
    printf("output section\n"); 
    printf("f4 IK: "); 
    for (i=0;i<L_IK;i++) 
        printf("%02x ",IK[i]); 
    printf("\n"); 

    pause(); 

    printf("\n"); 
    printf("\n"); 
    printf("test vector for f5\n"); 
    printf("input section\n"); 
    printf("K is: "); 
    for(i=0;i<L_KEY;i++) 
        printf("%02x ",K[i]); 
    printf("\n"); 
    printf("fi5 is: %02x\n",fi5); 
    printf("RAND is: "); 
    for(i=0;i<L_RAND;i++) 
        printf("%02x ",RAND[i]); 
    printf("\n"); 
    printf("Fmk is: "); 
    for(i=0;i<L_FMK;i++) 
        printf("%02x ",Fmk[i]); 
    printf("\n"); 
    printf("\n"); 
    f5(K,fi5,RAND,Fmk,AK); 
    printf("output section\n"); 
    printf("f5 AK: "); 
    for (i=0;i<L_AK;i++) 
        printf("%02x ",AK[i]); 
    printf("\n"); 

    pause(); 

    printf("\n"); 
    printf("\n"); 
    printf("test vector for f5*\n"); 
    printf("input section\n"); 
    printf("K is: "); 
    for(i=0;i<L_KEY;i++) 
        printf("%02x ",K[i]); 
    printf("\n"); 
    printf("fi5* is: %02x\n",fi5star); 
    printf("RAND is: "); 
    for(i=0;i<L_RAND;i++) 
        printf("%02x ",RAND[i]); 
    printf("\n"); 
    printf("Fmk is: "); 
    for(i=0;i<L_FMK;i++) 
        printf("%02x ",Fmk[i]); 
    printf("\n"); 
    printf("\n"); 
    f5star(K,fi5star,RAND,Fmk,AKS); 
    printf("output section\n"); 
    printf("f5* AKS: "); 
    for (i=0;i<L_AKS;i++) 
        printf("%02x ",AKS[i]); 
    printf("\n"); 

    pause(); 

    printf("\n"); 
    printf("\n"); 
    printf("test vector for f11\n"); 
    printf("input section\n"); 
    printf("K is: "); 
    for(i=0;i<L_KEY;i++) 
        printf("%02x ",K[i]); 
    printf("\n"); 
    printf("fi11 is: %02x\n",fi11); 
    printf("RAND is: "); 
    for(i=0;i<L_RAND;i++) 
        printf("%02x ",RAND[i]); 
    printf("\n"); 
    printf("Fmk is: "); 
    for(i=0;i<L_FMK;i++) 
        printf("%02x ",Fmk[i]); 
    printf("\n"); 
    printf("\n"); 
    f11(K,fi11,RAND,Fmk,UAK); 
    printf("output section\n"); 
    printf("f11 UAK: "); 
    for (i=0;i<L_UAK;i++) 
        printf("%02x ",UAK[i]); 
    printf("\n"); 

    pause(); 

} 
