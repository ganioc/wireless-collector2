#ifndef __MYE2PROM_H_
#define __MYE2PROM_H_

#include "stm32f1xx_hal.h"
#include "main.h"
#include "encrypt.h"
/**

8KB  = 64bit

there are 32 sections
1 section has 8 pages
1 page has 32 bytes

0 section - 0 page will store MBR info

0 section  - 1 page will store system information , <=32 bytes

**/

#define SECTION_MBRINFO      0
#define SECTION_SYSINFO      0
#define SECTION_RS485INFO    0
#define SECTION_ADVANCEINFO  0
#define SECTION_SECRETINFO   0

#define PAGE_MBRINFO          0
#define PAGE_SYSINFO          1
#define PAGE_RS485INFO        2
#define PAGE_ADVANCEINFO      3
#define PAGE_SECRETINFO       4

#define E2PROM_ADDR          0xA0
#define E2PROM_MAXPKT        32
#define E2PROM_WRITE_WAIT    10  //ms
#define E2PROM_READ_WAIT     10
#define E2PROM_TIMEOUT       5*E2PROM_WRITE_WAIT
#define E2PROM_SECTION_SIZE  32

#define MAX_SECRET_KEY_LEN   16
#define SECRET_KEY           "0123456789ruffos"
// this is the key to encrypt secret_key durint broadcasting
#define FACTORY_KEY          "0011223344556677"   
#define MAGIC_NUM            'R'  

typedef struct MBRInfo
{
    char mark1; // Whether it's a valid info, 'Ruff' is correct, 0xFF is not;
    char mark2;
    char mark3;
    char mark4;
} MBRInfo_t;

typedef struct SysInfo
{
    uint8_t addrH;
    uint8_t addrL;
    uint8_t sped;
    uint8_t chan;
    uint8_t option;
    
    uint8_t role;  // 1 master, 0 slave
    char model[20];
    char version[5];


} SysInfo_t;

typedef struct Rs485Info{
    uint8_t baudRate;
    uint8_t parity; 
    uint8_t stopBit;
    
}Rs485Info_t;

typedef struct AdvanceInfo{
    uint8_t packetDelayH;
    uint8_t packetDelayL;
    uint8_t bEncrypt;
    uint8_t secretKey[MAX_SECRET_KEY_LEN+1];
        
}AdvanceInfo_t;

typedef struct SecretInfo{
    uint8_t factoryKey[MAX_SECRET_KEY_LEN+1];
}SecretInfo_t;

typedef struct BroadcastingMessage{
    uint8_t magicNum;
    uint8_t channel;
    uint8_t factoryKey[MAX_SECRET_KEY_LEN+1];
    uint8_t baudRate;
    uint8_t parity; 
    uint8_t stopBit;    
}BroadcastingMessage_t;

#define MBRINFO_SIZE            sizeof(MBRInfo_t)
#define SYSINFO_SIZE            sizeof(SysInfo_t)
#define RS485INFO_SIZE          sizeof(Rs485Info_t)
#define ADVANCEINFO_SIZE        sizeof(AdvanceInfo_t)
#define SECRETINFO_SIZE         sizeof(SecretInfo_t)
#define BROADCASTINGMESSAGE_SIZE sizeof(BroadcastingMessage_t)

void E2PROM_Init(void);

SysInfo_t* getSysInfoPointer();
void saveSysInfoPointer();

Rs485Info_t* getRs485InfoPointer();
void saveRs485InfoPointer();

AdvanceInfo_t * getAdvanceInfoPointer();
void saveAdvanceInfoPointer();

uint8_t getSysInfoChannel();
uint8_t getSysInfoRole();    

void ResetToDefaultE2Prom();
uint16_t getPacketDelay();

uint8_t isUseEncrypt();
void getSecretKey(uint8_t in[]);
void getFactoryKey(uint8_t in[]);
void saveFactoryKey(uint8_t in[]);

void saveSecretInfoPointer();
SecretInfo_t * getSecretInfoPointer();

uint16_t getAddress();
uint8_t getChannel();
void saveChannel(uint8_t ch);

void saveRs485Params(uint8_t baud, uint8_t parity, uint8_t stopBit);
void getEncryptedFactoryKey(uint8_t in[]);
void getDecryptedFactoryKey(uint8_t in[]);
#endif
