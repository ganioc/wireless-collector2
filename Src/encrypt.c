#include "encrypt.h"
#include <stdio.h>
#include "stm32f1xx_hal.h"
#include "mye2prom.h"
#include "ecb.h"


static uint8_t SECRETS_KEY[MAX_SECRET_KEY_LEN]= {0};

static uint8_t FACTORYS_KEY[MAX_SECRET_KEY_LEN] = {0};

static uint8_t tempBuf[320] = {0};

void Encrypt_Init()
{
    int i =0;
    getSecretKey(SECRETS_KEY);
    getFactoryKey(FACTORYS_KEY);

    for(i=0; i< MAX_SECRET_KEY_LEN; i++)
    {
        printf("0x%2x ", SECRETS_KEY[i]);
    }
    printf("\r\n");

    for(i=0; i< MAX_SECRET_KEY_LEN; i++)
    {
        printf("0x%2x ", FACTORYS_KEY[i]);
    }
    printf("\r\n");    
}

// return buf size
int encrypt(char buf[], int len, uint8_t SECRETS[])
{
    int lenOut = 0, lenPadding;
    int i, numPadding, remain, index=0;
//    uint8_t inTemp[MAX_SECRET_KEY_LEN]; // 16
//    uint8_t outTemp[MAX_SECRET_KEY_LEN];

    remain = len%MAX_SECRET_KEY_LEN;

    if(remain <= 13)
    {
        lenPadding = MAX_SECRET_KEY_LEN - remain;
    }
    else
    {
        lenPadding = MAX_SECRET_KEY_LEN*2 - remain;
    }

    lenOut = lenPadding + len;
    printf("len:%d  lenPadding:%d  lenOut:%d \r\n",len, lenPadding, lenOut);

    // create padding
    tempBuf[index++] = 'R';
    tempBuf[index++] = 'U';
    numPadding = lenPadding - 3;
    tempBuf[index++] = numPadding;

    printf("numPadding:%d \r\n",numPadding);

    for(i=0; i< numPadding; i++)
    {
        tempBuf[index++] = 0x0;
    }

    // appending len
    for(i=0; i <len; i++)
    {
        tempBuf[index++] = buf[i];
    }

    for(i = 0; i< lenOut/MAX_SECRET_KEY_LEN; i++)
    {
        AES128_ECB_encrypt(tempBuf + i*MAX_SECRET_KEY_LEN, SECRETS, (uint8_t*)(buf+ i*MAX_SECRET_KEY_LEN));
    }

    return lenOut;
}

// return buf size,
int decrypt(char buf[], int len, uint8_t SECRETS[])
{
    int lenOut = 0, i;
    int remain = len % MAX_SECRET_KEY_LEN;


    if(remain != 0)
    {
        printf("Wrong len of packet to be decrypted\r\n");
        return -1;
    }

    printf("decrypt len:%d, block size:%d\r\n", len, MAX_SECRET_KEY_LEN);
    for(i=0; i< len; i++)
    {
        //printf("0x%02x ", buf[i]);
    }
    printf("\r\n");

    for(i = 0; i< len/MAX_SECRET_KEY_LEN; i++)
    {
        AES128_ECB_decrypt((uint8_t*)buf + i*MAX_SECRET_KEY_LEN, SECRETS, (uint8_t*)tempBuf+ i*MAX_SECRET_KEY_LEN);
    }

    printf("decrypt out:\r\n");
    for(i=0; i< len; i++)
    {
        //printf("0x%02x ", tempBuf[i]);
    }
    printf("\r\n");

    if(tempBuf[0]!= 'R' || tempBuf[1]!= 'U')
    {
        printf("Unmatched RU header\r\n");

        return -1;
    }

    for(i = 0; i< tempBuf[2]; i++){
        if(tempBuf[3+i] != 0x00){
            printf("Wrong padding value not 0x00\r\n");
            return -1;
        }
    }

    lenOut = len -tempBuf[2] - 3;

    for(i=0; i< lenOut; i++)
    {
        buf[i] = tempBuf[tempBuf[2] + 3 +i];
    }

    return lenOut;
}
int encryptSecret(char buf[], int len){
    return encrypt(buf, len, SECRETS_KEY);
}
int encryptFactory(char buf[], int len){
    return encrypt(buf,len, FACTORYS_KEY);
}
int decryptSecret(char buf[], int len){
    return decrypt(buf,len, SECRETS_KEY);
}
int decryptFactory(char buf[], int len){
    return decrypt(buf,len,FACTORYS_KEY);
}
void testEncrypt2()
{

    uint8_t inBuf[] = { 0x11, 0x22, 0x33, 0x44,0x55, 0x66, 0x77, 0x88, 0x11, 0x22, 0x33, 0x44,0x55, 0x66, 0x77, 
    0x88,0x99, 0xaa, 0xbb, 0xcc, 0x11, 0x22, 0x33, 0x44,0x55, 0x66, 0x77, 0x88, 0x11, 0x22, 0x33, 0x44,0x55, 0x66, 0x77, 
    0x88,0x99, 0xaa, 0xbb, 0xcc,0x11, 0x22, 0x33, 0x44,0x55, 0x66, 0x77, 0x88, 0x11, 0x22, 0x33, 0x44,0x55, 0x66, 0x77, 
    0x88,0x99, 0xaa, 0xbb, 0xcc, 0x11, 0x22, 0x33, 0x44,0x55, 0x66, 0x77, 0x88, 0x11, 0x22, 0x33, 0x44,0x55, 0x66, 0x77, 
    0x88,0x99, 0xaa, 0xbb, 0xcc, 0x11, 0x22, 0x33, 0x44,0x55, 0x66, 0x77, 0x88, 0x11, 0x22, 0x33, 0x44,0x55, 0x66, 0x77, 
    0x88,0x99, 0xaa, 0xbb, 0xcc,0x11, 0x22, 0x33, 0x44,0x55, 0x66, 0x77, 0x88, 0x11, 0x22, 0x33, 0x44,0x55, 0x66, 0x77, 
    0x88,0x99, 0xaa, 0xbb, 0xcc, 0x11, 0x22, 0x33, 0x44,0x55, 0x66, 0x77, 0x88, 0x11, 0x22, 0x33, 0x44,0x55, 0x66, 0x77, 
    0x88,0x99, 0xaa, 0xbb, 0xcc, 0x11, 0x22, 0x33, 0x44,0x55, 0x66, 0x77, 0x88, 0x11, 0x22, 0x33, 0x44,0x55, 0x66, 0x77, 
    0x88,0x99, 0xaa, 0xbb, 0xcc,0x11, 0x22, 0x33, 0x44,0x55, 0x66, 0x77, 0x88, 0x11, 0x22, 0x33, 0x44,0x55, 0x66, 0x77, 
    0x88,0x99, 0xaa, 0xbb, 0xcc,0x11, 0x22, 0x33, 0x44,0x55, 0x66, 0x77, 0x88, 0x11, 0x22, 0x33, 0x44,0x55, 0x66, 0x77, 
    0x88,0x99, 0xaa, 0xbb, 0xcc,0x11, 0x22, 0x33, 0x44,0x55, 0x66, 0x77, 0x88, 0x11, 0x22, 0x33, 0x44,0x55, 0x66, 0x77, 
    0x88,0x99, 0xaa, 0xbb, 0xcc,0x11, 0x22, 0x33, 0x44,0x55, 0x66, 0x77, 0x88, 0x11, 0x22, 0x33, 0x44,0x55, 0x66, 0x77, 
    0x88,0x99, 0xaa, 0xbb, 0xcc,0x11, 0x22, 0x33, 0x44,0x55, 0x66, 0x77, 0x88, 0x11, 0x22, 0x33, 0x44,0x55, 0x66, 0x77, 
    0x88,0x99, 0xaa, 0xbb, 0xcc,0x11, 0x22, 0x33,
                      };
    uint8_t in[sizeof(inBuf) + 20] = {0};
    int len= sizeof(inBuf) ;
    int i;

    printf("\r\nin put data is len: %d\r\n", len);

    for(i=0; i< len; i++)
    {
        in[i] = inBuf[i];
        printf("0x%02x ", in[i]);
    }
    printf("\r\n\r\n");

    len = encrypt((char *) in, len, FACTORYS_KEY);

    printf("out len:%d\r\n", len);

    for(i=0; i< len; i++)
    {
        printf("0x%02x ", in[i]);

    }
    printf("\r\n");

    len = decrypt((char*)in, len, FACTORYS_KEY);

    if(len != -1)
    {
        printf("\r\ndecrypted result:\r\n");
        for(i=0; i< len; i++)
        {
            printf("0x%02x ", in[i]);

        }
        printf("\r\n");

    }


}

void testEncrypt()
{
    uint8_t inBuf[] = { 0x11, 0x22, 0x33, 0x44,0x55, 0x66, 0x77, 0x88, 0x11, 0x22, 0x33, 0x44,0x55, 0x66, 0x77, 0x88,
                      };
    uint8_t outBuf[16]= {0};
    int i;

    printf("SECRETS %d\r\n", 16);
    for(i =0 ; i< 16; i++)
    {
        printf("0x%02x ", SECRETS_KEY[i]);
    }
    printf("\r\n");

    printf("Input %d bytes data\r\n", sizeof(inBuf));
    for(i =0 ; i< 16; i++)
    {
        printf("0x%02x ", inBuf[i]);
    }
    printf("\r\n");
    AES128_ECB_encrypt(inBuf, SECRETS_KEY, outBuf);

    printf("Output bytes data\r\n");
    for(i =0 ; i< 16; i++)
    {
        printf("0x%02x ", outBuf[i]);
    }
    printf("\r\n");

    printf("Decrypt out\r\n");
    AES128_ECB_decrypt(outBuf, SECRETS_KEY, inBuf);
    for(i =0 ; i< 16; i++)
    {
        printf("0x%02x ", inBuf[i]);
    }


}

static void disp(uint8_t d[]){
    int i;
    for(i=0; i<16; i++){
        printf("0x%02x ", d[i]);
        
    }
    printf("\r\n");
}
void testEncrypt3(){
    uint8_t data[16], data2[16];
    printf("\r\n test 3\r\n");

    getFactoryKey(data);
    disp(data);
    printf("\r\n encrypt:\r\n");
    getEncryptedFactoryKey(data2);
    disp(data2);

    printf("decrypt it:\r\n");
    getDecryptedFactoryKey(data2);

    disp(data2);
}