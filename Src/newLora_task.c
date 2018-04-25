#include "newLora_task.h"
#include "cmsis_os.h"
#include "mye2prom.h"
#include "message.h"
#include "ctrllora.h"
#include "globalconfig.h"
#include "newRs485_task.h"
#include "myled.h"

extern TaskThread_t mLoraThread;
extern uint8_t bSlaveReceivedLoraCommand;
extern uint8_t srcAddressH;
extern uint8_t srcAddressL;

uint8_t mLoraState;

uint8_t RX_BUF_LORA[BUFFER_MAX_SIZE_RS485];
uint16_t indexRxLora = 0;

uint8_t RX_BUF_FOR_RS485[BUFFER_MAX_SIZE_RS485];
uint16_t indexRxForRs485 = 0;



void ConfigLora()
{
    uint8_t params[6];
    SysInfo_t *pSysInfo;

    pSysInfo = getSysInfoPointer();
    
    // if role == master
    if(pSysInfo->role == ROLE_MASTER)
    {
      params[1] = pSysInfo->addrH;
      params[2] = pSysInfo->addrL;
    } // role == slave
    else{
      params[1] = 0x12;
      params[2] = 0xef;
    }
    
    params[0] = 0xC2;

    params[3] = pSysInfo->sped;
    params[4] = pSysInfo->chan;
    params[5] = pSysInfo->option;

    WriteLora(params, 6);

    printf("config data:0x%02x %02x %02x %02x %02x %02x\r\n",
           params[0], params[1], params[2], params[3], params[4], params[5]);
}
void ConfigLoraChannel(uint8_t ch)
{
    uint8_t params[6];
    SysInfo_t *pSysInfo;

    pSysInfo = getSysInfoPointer();

    params[0] = 0xC2;
    
    
//    params[1] = pSysInfo->addrH;
//    params[2] = pSysInfo->addrL;
    
    // if role == master
    if(pSysInfo->role == ROLE_MASTER)
    {
      params[1] = pSysInfo->addrH;
      params[2] = pSysInfo->addrL;
    } // role == slave
    else{
      params[1] = 0x12;
      params[2] = 0xef;
    }
    
    
    params[3] = pSysInfo->sped;
    params[4] = ch;
    params[5] = pSysInfo->option;

    WriteLora(params, 6);

    printf("config data:0x%02x %02x %02x %02x %02x %02x\r\n",
           params[0], params[1], params[2], params[3], params[4], params[5]);
}
uint8_t bExactLen(uint8_t lenH, uint8_t lenL, uint8_t lenPacket)
{
    if ((lenH << 8 | lenL) == lenPacket)
    {
        return 1;
    }

    return 0;
}
void HandleData(uint8_t *inBuf, uint8_t inLen, void (*handle)(uint8_t *buf, uint16_t len, uint8_t ch), uint8_t channel)
{
    uint8_t i;
    uint16_t len16;

    if (inLen <= MAX_DATA_LENGTH && inLen > 9 && inBuf[0] == 0x3a && inBuf[inLen - 1] == 0x0a && inBuf[inLen - 2] == 0x0d && bExactLen(inBuf[5], inBuf[6], inLen - 9)) // Whole packet
    {

        if (GetRole() == SLAVE)
        {
            // addr16LastTime = inBuf[1]<< 8| inBuf[2];
            bSlaveReceivedLoraCommand = 1;
            srcAddressH = inBuf[1];
            srcAddressL = inBuf[2];
        }

        //SendOutRs485Data(inBuf + 7, inBuf[5] << 8 | inBuf[6]);
        handle(inBuf + 7, inBuf[5] << 8 | inBuf[6], channel);
    }
    else if (inLen < MAX_DATA_LENGTH) // Last packet
    {
        for (i = 0; i < inLen; i++)
        {
            RX_BUF_FOR_RS485[indexRxForRs485++] = inBuf[i];
        }
        len16 = RX_BUF_FOR_RS485[5] << 8 | RX_BUF_FOR_RS485[6];

        //SendOutRs485Data(RX_BUF_FOR_RS485 + 7, len16);
        handle(RX_BUF_FOR_RS485 + 7, len16, channel);

        if (GetRole() == SLAVE)
        {
            bSlaveReceivedLoraCommand = 1;
        }

        indexRxForRs485 = 0;
    }
    else if (inLen == MAX_DATA_LENGTH && inBuf[0] == 0x3a && inBuf[1] == 0x00) // First packet
    {
        if (GetRole() == SLAVE)
        {
            //addr16LastTime = inBuf[1]<< 8| inBuf[2];
            bSlaveReceivedLoraCommand = 1;
            srcAddressH = inBuf[1];
            srcAddressL = inBuf[2];
        }
        indexRxForRs485 = 0;

        for (i = 0; i < MAX_DATA_LENGTH; i++)
        {
            RX_BUF_FOR_RS485[indexRxForRs485++] = inBuf[i];
        }
    }
    else if (inLen == MAX_DATA_LENGTH) // Middle packet
    {
        for (i = 0; i < MAX_DATA_LENGTH; i++)
        {
            RX_BUF_FOR_RS485[indexRxForRs485++] = inBuf[i];
        }
    }
    else
    {
        printf("Invalid data received\r\n");
    }
}
static void HandleLoraBytes(uint8_t *inBuf, uint8_t inLen, uint8_t channel)
{
    HandleData(inBuf, inLen, SendOutRs485Data, channel);
}
static void AnalyzeBroadcastingMessage(uint8_t *buf, uint16_t len, uint8_t channel)
{
    //int i =0;
    // int lenOut;
    BroadcastingMessage_t *pMsg = (BroadcastingMessage_t *)buf;
    printf("AnalyzeBroadcastingMessage %d\r\n", len);

    printf("Magic %c\r\n", pMsg->magicNum);
    if (pMsg->magicNum != 'R')
    {
        return;
    }
    // if channel is not the same
    printf("Compare channel my:%d, got:%d\r\n", channel, pMsg->channel);
    if(channel != pMsg->channel){
        return;
    }

    saveChannel(channel);
    // save factoryKey
    if(isUseEncrypt() == 1){
        //lenOut = decryptSecret((char *)pMsg->factoryKey, MAX_SECRET_KEY_LEN);
    }

    // for(i=0; i< lenOut; i++){
    //     printf("0x%02x  ", pMsg->factoryKey[i]);
    // }
    printf("\r\n");

    saveFactoryKey(pMsg->factoryKey);

    // save baudRate, parity, and stopBit
    saveRs485Params(pMsg->baudRate, pMsg->parity, pMsg->stopBit);

    // saveRs485InfoPointer();
    // saveSecretInfoPointer();
    SwitchToSync();

}
// It will be a short message no more than 20 bytes
static void HandleLoraBroadcasting(uint8_t *inBuf, uint8_t inLen, uint8_t channel)
{
    printf("handle Broadcasting %d\r\n", inLen);
    // It is a limited-size packet
    HandleData(inBuf, inLen, AnalyzeBroadcastingMessage, channel);
}
void LoraUartHandler(void (*parser)(uint8_t *inBuf, uint8_t inLen, uint8_t ch), uint8_t channel)
{
    osEvent ret;
    uint16_t i;

    // waiting for the comming characters
    ret = osSignalWait(0x3, 50);

    if (ret.status == osEventSignal && ret.value.v == 1 && mLoraState == LORA_STATE_RX_NONE)
    {
        printf("lorathread signal 1 rx\r\n");
        mLoraState = LORA_STATE_RX_WAITING;
    }
    else if (ret.status == osEventSignal && ret.value.v == 2)
    {
        printf("lorathread signal 2 rx\r\n");
    }
    else if (ret.status == osEventTimeout && mLoraState == LORA_STATE_RX_WAITING)
    {
        // frame ended
        // send it out

        printf("\r\nGet data from Lora, len:%d\r\n", indexRxLora);

        for (i = 0; i < indexRxLora; i++)
        {
            printf("0x%02x ", RX_BUF_LORA[i]);
        }
        printf("\r\n");

        // send it to RS485 port
        if (indexRxLora <= 4)
        {
            printf("too few bytes\r\n");
        }
        else
        {
            parser(RX_BUF_LORA, indexRxLora, channel);
        }

        // Housekeeping
        indexRxLora = 0;

        mLoraState = LORA_STATE_RX_NONE;

        // toggle led2
        // FlashLED2();
    }
    else if (ret.status == osEventTimeout && mLoraState == LORA_STATE_RX_NONE)
    {
    }
}
void LoraConfigByteHandler(uint8_t c)
{
}
void LoraWorkingByteHandler(uint8_t c)
{
    RX_BUF_LORA[indexRxLora++] = c;
    osSignalSet(mLoraThread.idThread, 0x01);
}
void LoraBroadcastingByteHandler(uint8_t c)
{
    RX_BUF_LORA[indexRxLora++] = c;
    osSignalSet(mLoraThread.idThread, 0x01);
}
void LoraWaitingByteHandler(uint8_t c)
{
}
void LoraSyncByteHandler(uint8_t c)
{
}
void LoraConfigTask()
{
    osDelay(2000);
    //printf("\r\nlora-config\r\n");
}

void LoraWorkingTask()
{
    LoraUartHandler(HandleLoraBytes, 0);
}
void LoraBroadcastingMaster()
{
    while (1)
    {
        osDelay(2000);
        //printf("lora-broadcasting-master\r\n");
    }
}
void SwitchChannel(uint8_t channel)
{
    printf("\r\n=====> Switch to channel:%d\r\n", channel);
    //
    while (bGetAuxKey() == 0)
    {
        osDelay(5);
    }
    SetLoraSettingMode();
    while (bGetAuxKey() == 0)
    {
        osDelay(5);
    }
    osDelay(5);
    ConfigLoraChannel(channel);
    while (bGetAuxKey() == 0)
    {
        osDelay(5);
    }
    osDelay(5);
    SetLoraWorkingMode();
    osDelay(50);
    mLoraState = LORA_STATE_RX_NONE;
    indexRxLora = 0;
}

void LoraBroadcastingSlave()
{
    uint16_t counter = 0;
    uint8_t channel = 0;

    do
    {
        if (counter == 0)
        {
            
            if (channel >= 32)
            {
                channel = 0;
            }
            SwitchChannel(channel);
            channel++;
        }
        LoraUartHandler(HandleLoraBroadcasting, channel-1);
        counter++;
        if (counter >= 70)
        {
            counter = 0;
        }
        if(GetStateContext().type != 2){
            printf("Breaking into Sync state\r\n");
            break;
        }

    } while (1);
}
void LoraBroadcastingTask()
{
    if (GetRole() == SLAVE)
    {
        LoraBroadcastingSlave();
    }
    else
    {
        LoraBroadcastingMaster();
    }
}
void LoraWaitingTask()
{
    osDelay(2000);
    //printf("\r\nlora-waiting\r\n");
}
void LoraSyncTask()
{
    SecretInfo_t *pSecretInfo = getSecretInfoPointer();
    int i;
    // save all of it here
    for(i=0; i< MAX_SECRET_KEY_LEN; i++){
        printf("0x%02x ", pSecretInfo->factoryKey[i]);
    }
    printf("\r\n");
    
    getDecryptedFactoryKey(pSecretInfo->factoryKey);

    osDelay(50);

    for(i=0; i< MAX_SECRET_KEY_LEN; i++){
        printf("0x%02x ", pSecretInfo->factoryKey[i]);
    }
    printf("\r\n");

    saveRs485InfoPointer();
    // It is factoryKey instead
    saveSecretInfoPointer();
    saveSysInfoPointer();

    osDelay(100);

    while(1){
        osDelay(2000);
        printf("\r\nlora-sync\r\n");
    }

}