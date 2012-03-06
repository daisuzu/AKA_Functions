/* aka.h */ 
/* header for SHA based AKA functions */ 

#ifndef AKA_H 
#define AKA_H 

#include "sha.h" 

typedef unsigned char uchar; 
typedef unsigned short word16; 
typedef unsigned long int word32; 

#define L_KEY 16 /*128 bits 3G TS 33.105.v.3.0(2000-03)*/ 
#define L_RAND 16 /*128 bits 3G TS 33.105.v.3.0(2000-03)*/ 
#define L_FMK 4 /*32 bits */ 
#define L_SQN 6 /*48 bits 3G TS 33.105.v.3.0(2000-03)*/ 
#define L_AMF 2 /*16 bits 3G TS 33.105.v.3.0(2000-03)*/ 
#define L_MACA 8 /*64 bits 3G TS 33.105.v.3.0(2000-03)*/ 
#define L_MACS 8 /*64 bits 3G TS 33.105.v.3.0(2000-03)*/ 
#define L_RES 16 /*32<->128 bits 3G TS 33.105.v.3.0(2000-03)*/ 
#define L_CK 16 /*128 bits 3G TS 33.105.v.3.0(2000-03)*/ 
#define L_IK 16 /*128 bits 3G TS 33.105.v.3.0(2000-03)*/ 
#define L_AK 6 /*48 bits 3G TS 33.105.v.3.0(2000-03)*/ 
#define L_AKS 6 /*48 bits 3G TS 33.105.v.3.0(2000-03)*/ 
#define L_UAK 16 

/* function definitions */ 

void f0(uchar seed[],uchar fi,uchar Fmk[],uchar buff[]); 
void f1(uchar K[],uchar fi,uchar *RAND,uchar Fmk[],uchar SQN[],uchar AMF[],uchar 
        MACA[]); 
void f1star(uchar K[],uchar fi,uchar *RAND,uchar Fmk[],uchar SQN[],uchar 
        AMF[],uchar MACS[]); 
void f2(uchar K[],uchar fi,uchar *RAND,uchar Fmk[],uchar RES[],int l_res); 
void f3(uchar K[],uchar fi,uchar *RAND,uchar Fmk[],uchar *CK); 
void f4(uchar K[],uchar fi,uchar *RAND,uchar Fmk[],uchar *IK); 
void f5(uchar K[],uchar fi,uchar *RAND,uchar Fmk[],uchar AK[]); 
void f5star(uchar K[],uchar fi,uchar *RAND,uchar Fmk[],uchar AKS[]); 
void f11(uchar K[],uchar fi,uchar *RAND,uchar Fmk[],uchar *UAK); 

#endif 
