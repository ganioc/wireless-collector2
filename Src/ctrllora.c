#include "ctrllora.h"
#include "gpio.h"
#include "usart.h"
#include "mye2prom.h"
#include "frame.h"
#include "cmsis_os.h"
#include "encrypt.h"
#include "message.h"
#include "thread_crypto.h"

extern TaskThread_t mCryptoThread;
extern uint8_t CRYPTO_BUF[];
extern uint16_t indexCRYPTO;

uint8_t buf[512];

void SetLoraSettingMode()
{

    HAL_GPIO_WritePin(LORA_M_PORT, LORA_M0, GPIO_PIN_SET);
    HAL_GPIO_WritePin(LORA_M_PORT, LORA_M1, GPIO_PIN_SET);
}

void SetLoraWorkingMode()
{

    HAL_GPIO_WritePin(LORA_M_PORT, LORA_M0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LORA_M_PORT, LORA_M1, GPIO_PIN_RESET);
}

void WriteLora(uint8_t *buf, uint8_t len)
{
    uint8_t i;
    printf("writeLora len:%d, addr:%02x %02x channel:%d\r\n", len, buf[0], buf[1], buf[2]);

    for (i = 0; i < len; i++)
    {
        //printf("%02d: 0x%02x\r\n", i,  buf[i]);
    }

    UART2_Transmit(buf, len);
}
void WriteLoraConfig(uint8_t *buf, uint8_t len)
{
}

void WriteLoraData(uint8_t *buf, uint8_t len)
{
}

void SendOutLoraData(uint8_t bEncrypt, uint16_t addr, uint8_t *inBuf, uint16_t inLen)
{
    SysInfo_t *pSysInfo = getSysInfoPointer();
    uint8_t channel = pSysInfo->chan;
    uint16_t index = 0;
    uint16_t i,nTotal, lenEncrypt;

    printf("\r\nSendOut Lora data  to addr: %d at channel %d  len:%d\r\n", addr,
           channel, inLen);

    buf[index++] = FRAME_HEAD;
    buf[index++] = pSysInfo->addrH;
    buf[index++] = pSysInfo->addrL;
    buf[index++] = 0xff & (addr >> 8);
    buf[index++] = 0xff & (addr);

    // encrypt buf
    if (bEncrypt == 1)
    {
        lenEncrypt = encryptFactory((char *)inBuf, inLen);
        printf("encrypted len:%d\r\n", lenEncrypt);
        printf(">>8  %d\r\n", (lenEncrypt >> 8) & 0xff);
        printf("&&8  %d\r\n", (lenEncrypt)&0xff);

        buf[index++] = (lenEncrypt >> 8) & 0xff;
        buf[index++] = lenEncrypt & 0xff;

        for (i = 0; i < lenEncrypt; i++)
        {
            buf[index++] = inBuf[i];
        }
    }
    else
    {
        buf[index++] = 0x00;
        buf[index++] = inLen;
        for (i = 0; i < inLen; i++)
        {
            buf[index++] = inBuf[i];
        }
    }

    buf[index++] = FRAME_TAIL_0;
    buf[index++] = FRAME_TAIL_1;

    nTotal = index;

    for (i = 0; i < nTotal; i++)
    {
        printf("0x%02x ", buf[i]);
        CRYPTO_BUF[i] = buf[i];
    }
    indexCRYPTO = nTotal;
    printf("\r\n");

    osSignalSet(mCryptoThread.idThread, 0x02);

    /*
    headerBuf[0] = 0xff; // & (addr >> 8);
    headerBuf[1] = 0xff; // & (addr);
    headerBuf[2] = channel;

    printf("total length:%d\r\n", nTotal);

    for (i = 0; i < nTotal; i += MAX_DATA_LENGTH)
    {

        nTemp = (nTotal < i + MAX_DATA_LENGTH) ? nTotal : i + MAX_DATA_LENGTH;
        printf("No of send:%d, nTemp:%d\r\n", i, nTemp);

        for (j = 0; j < nTemp - i; j++)
        {

            headerBuf[3 + j] = buf[i + j];
        }
        WriteLora(headerBuf, nTemp - i + 3);

        osDelay(10);
        printf("End of send. %d\r\n", i);
    }
*/
}

void SendOutRs485Data(uint8_t *buf, uint16_t len, uint8_t channel)
{
    uint16_t i;
    //int lenOut;

    printf("\r\nSendout to RS485: len: %d\r\n", len);

    for (i = 0; i < len; i++)
    {
        printf("0x%02x ", buf[i]);
    }
    printf("\r\n");

    // find out the data
    // decrypt it
    for (i = 0; i < len; i++)
    {
        CRYPTO_BUF[i] = buf[i];
    }
    // lenOut = decryptFactory((char*)buf, len);
    indexCRYPTO = len;

    if (isUseEncrypt() == 1)
    {

        osSignalSet(mCryptoThread.idThread, 0x01);
        //UART3_Transmit(buf, lenOut);
    }
    else
    {

        //UART3_Transmit(buf, len);
        // for (i = 0; i < len; i++)
        // {
        //     CRYPTO_BUF[i] = buf[i];
        // }
        // // lenOut = decryptFactory((char*)buf, len);
        // indexCRYPTO = len;
        osSignalSet(mCryptoThread.idThread, 0x04);
    }
}
