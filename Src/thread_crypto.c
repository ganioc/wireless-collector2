#include "thread_crypto.h"
#include "message.h"
#include "stdio.h"
#include "usart.h"
#include "string.h"
#include "mye2prom.h"
#include "parser.h"
#include "ctrllora.h"
#include "myled.h"
#include "cmsis_os.h"
#include "encrypt.h"

TaskThread_t mCryptoThread;
uint8_t CRYPTO_BUF[280];
uint16_t indexCRYPTO;

uint8_t headerBuf[65];

static void TaskLoop(void const *argument)
{

    osEvent ret;
    int lenDecryp, i, nTotal , nTemp,j;
    SysInfo_t *pSysInfo = getSysInfoPointer();
    uint8_t channel = pSysInfo->chan;
    // SysInfo_t *pSysInfo ;

    while (1)
    {
        // waiting for the comming characters
        ret = osSignalWait(0x7, 200);

        if (ret.status == osEventSignal && ret.value.v == 1)
        {
            lenDecryp = decryptFactory((char *)CRYPTO_BUF, indexCRYPTO);
            printf("lenDecryp:%d\r\n", lenDecryp);

            if (lenDecryp == -1)
            {
                printf("Wrong decrypt result\r\n");
            }
            else
            {
                printf("\r\nCorrect\r\n");
                for (i = 0; i < lenDecryp; i++)
                {
                    printf("0x%02x ", CRYPTO_BUF[i]);
                }
                printf("\r\n");
                UART3_Transmit(CRYPTO_BUF, lenDecryp);

                
            }
        }
        else if (ret.status == osEventSignal && ret.value.v == 2)
        {
            printf("\r\nthread_crypto::received rs485 data %d\r\n", indexCRYPTO);

            nTotal = indexCRYPTO;

            // if role == master
            if(pSysInfo->role == ROLE_MASTER)
            {
              headerBuf[0] = 0xff; // & (addr >> 8);
              headerBuf[1] = 0xff; // & (addr);
            }
            // else role == slave, send it only to the master
            else{
              headerBuf[0] = pSysInfo->addrH; // & (addr >> 8);
              headerBuf[1] = pSysInfo->addrL; // & (addr);
            }
            

            headerBuf[2] = channel;

            printf("total length:%d  channel:%d\r\n", nTotal, channel);

            for (i = 0; i < nTotal; i += MAX_DATA_LENGTH)
            {

                nTemp = (nTotal < i + MAX_DATA_LENGTH) ? nTotal : i + MAX_DATA_LENGTH;
                printf("\r\nNo of send:%d, nTemp:%d\r\n", i, nTemp);

                for (j = 0; j < nTemp - i; j++)
                {

                    headerBuf[3 + j] = CRYPTO_BUF[i + j];
                }
                WriteLora(headerBuf, nTemp - i + 3);

                osDelay(10);
                printf("End of send. %d\r\n", i);
            }
        }
        else if (ret.status == osEventSignal && ret.value.v == 4)
        {
            printf("\r\nthread_crypto::received lora data %d\r\n", indexCRYPTO);
            UART3_Transmit(CRYPTO_BUF, indexCRYPTO);
        }
        else if (ret.status == osEventTimeout)
        {
        }
    }
}

void CryptoThreadInit()
{
    osThreadDef(cryptothread, TaskLoop, osPriorityHigh, 0, 128);
    mCryptoThread.idThread = osThreadCreate(osThread(cryptothread), NULL);

    if (mCryptoThread.idThread == NULL)
    {

        printf("mCryptoThread create fail\r\n");
    }
    else
    {
        printf("mCryptoThread create OK\r\n");
    }
}