#include "parser.h"
#include <stdio.h>
#include <string.h>
#include "main.h"
#include "thread_rs485.h"
#include "message.h"
#include "mye2prom.h"
#include "usart.h"
#include <stdlib.h>

extern TaskThread_t mRs485Thread;

char strBuf[128];
char strTemp[16];

void parseConfigSet(char *str, uint8_t len)
{
    char strObj[5];
    char strContent[16];

    uint8_t i, j = 0, length;
    SysInfo_t *pSysInfo;
    Rs485Info_t *pRs485Info;
    AdvanceInfo_t *pAdvanceInfo;
    uint16_t addr16, channel;

    for (i = 0; i < 4; i++)
    {
        strObj[i] = str[i];
    }
    strObj[i] = '\0';

    for (; i < len; i++)
    {
        strContent[j++] = str[i];
    }
    strContent[j] = '\0';

    if (strcmp(strObj, "QUIT") == 0)
    {
        printf("Quit config\r\n");
        osSignalSet(mRs485Thread.idThread, 0x02);
    }
    else if (strcmp(strObj, "ADDR") == 0)
    {
        printf("address config\r\n");

        if (strlen(strContent) > 3 || strlen(strContent) == 0)
        {
            printf("wrong format of addr: %d\r\n", strlen(strContent));
            sprintf(strBuf, "%s\r\n", "SETNOK");
            UART3_Transmit((uint8_t *)strBuf, strlen(strBuf));
            return;
        }
        else
        {
            // save addr
            // strContent to number
            addr16 = atoi((char *)strContent);

            pSysInfo = getSysInfoPointer();
            pSysInfo->addrH = 0xff & (addr16 >> 8);
            pSysInfo->addrL = 0xff & addr16;

            printf("Set addr to:0x%x%x\r\n",
                   pSysInfo->addrH,
                   pSysInfo->addrL);
        }
    }
    else if (strcmp(strObj, "CHAN") == 0)
    {
        printf("channel config\r\n");
        if (strlen(strContent) > 3 || strlen(strContent) == 0)
        {
            printf("wrong format of chan: %d\r\n", strlen(strContent));
            sprintf(strBuf, "%s\r\n", "SETNOK");
            UART3_Transmit((uint8_t *)strBuf, strlen(strBuf));
            return;
        }
        else
        {
            // save addr
            pSysInfo = getSysInfoPointer();
            channel = atoi((char *)strContent);
            pSysInfo->chan = channel & 0xff;

            printf("Set channel to:0x%x\r\n",
                   pSysInfo->chan);
        }
    }
    else if (strcmp(strObj, "BAUD") == 0)
    {
        printf("RS485 portl baudrate config\r\n");
        if (strlen(strContent) > 3 || strlen(strContent) == 0)
        {
            printf("wrong format of baudrate: %d\r\n", strlen(strContent));
            sprintf(strBuf, "%s\r\n", "SETNOK");
            UART3_Transmit((uint8_t *)strBuf, strlen(strBuf));
            return;
        }
        else
        {
            // save baudrate
            pRs485Info = getRs485InfoPointer();
            channel = atoi((char *)strContent);
            pRs485Info->baudRate = channel & 0xff;

            printf("Set baudrate to:0x%x\r\n",
                   pRs485Info->baudRate);
        }
    }
    else if (strcmp(strObj, "PARI") == 0)
    {
        printf("RS485 portl parity config\r\n");
        if (strlen(strContent) > 3 || strlen(strContent) == 0)
        {
            printf("wrong format of parity: %d\r\n", strlen(strContent));
            sprintf(strBuf, "%s\r\n", "SETNOK");
            UART3_Transmit((uint8_t *)strBuf, strlen(strBuf));
            return;
        }
        else
        {
            // save parity
            pRs485Info = getRs485InfoPointer();
            channel = atoi((char *)strContent);
            pRs485Info->parity = channel & 0xff;

            printf("Set parity to:0x%x\r\n",
                   pRs485Info->parity);
        }
    }
    else if (strcmp(strObj, "DELY") == 0)
    {
        printf("RS485 packet delay config\r\n");
        if (strlen(strContent) > 3 || strlen(strContent) == 0)
        {
            printf("wrong format of packet delay: %d\r\n", strlen(strContent));
            sprintf(strBuf, "%s\r\n", "SETNOK");
            UART3_Transmit((uint8_t *)strBuf, strlen(strBuf));
            return;
        }
        else
        {
            // save delay
            pAdvanceInfo = getAdvanceInfoPointer();
            channel = atoi((char *)strContent);
            pAdvanceInfo->packetDelayH = (channel >> 8) & 0xff;
            pAdvanceInfo->packetDelayL = channel & 0xff;

            printf("Set packet delay to:0x%x  0x%x\r\n",
                   pAdvanceInfo->packetDelayH, pAdvanceInfo->packetDelayL);
        }
    }
    else if (strcmp(strObj, "STOP") == 0)
    {
        printf("RS485 portl stopbits config\r\n");
        if (strlen(strContent) > 3 || strlen(strContent) == 0)
        {
            printf("wrong format of stopbits: %d\r\n", strlen(strContent));
            sprintf(strBuf, "%s\r\n", "SETNOK");
            UART3_Transmit((uint8_t *)strBuf, strlen(strBuf));
            return;
        }
        else
        {
            // save stopbits
            pRs485Info = getRs485InfoPointer();
            channel = atoi((char *)strContent);
            pRs485Info->stopBit = channel & 0xff;

            printf("Set stopbits to:0x%x\r\n",
                   pRs485Info->stopBit);
        }
    }
    else if (strcmp(strObj, "SAVE") == 0)
    {
        printf("save to e2prom config\r\n");
        saveSysInfoPointer();
        saveRs485InfoPointer();
        saveAdvanceInfoPointer();
        sprintf(strBuf, "%s\r\n", "SAVEOK");
        UART3_Transmit((uint8_t *)strBuf, strlen(strBuf));
        return;
    }
    else if (strcmp(strObj, "MAST") == 0)
    {
        printf("set to master config\r\n");
        pSysInfo = getSysInfoPointer();
        pSysInfo->role = ROLE_MASTER;
    }
    else if (strcmp(strObj, "SLAV") == 0)
    {
        printf("set to slave config\r\n");
        pSysInfo = getSysInfoPointer();
        pSysInfo->role = ROLE_SLAVE;
    }
    else if (strcmp(strObj, "CRYP") == 0)
    {
        printf("set encrypt to:\r\n");
        if (strlen(strContent) != 1)
        {
            printf("wrong format of CRYP: %d\r\n", strlen(strContent));
            sprintf(strBuf, "%s\r\n", "SETNOK");
            UART3_Transmit((uint8_t *)strBuf, strlen(strBuf));
            return;
        }
        else
        {
            pAdvanceInfo = getAdvanceInfoPointer();
            j = atoi((char *)strContent);
            pAdvanceInfo->bEncrypt = j;
            printf("encrypt:%d\r\n", j);
        }
    }
    else if (strcmp(strObj, "KEYS") == 0)
    {
        printf("set KEYS to:\r\n");
        pAdvanceInfo = getAdvanceInfoPointer();
        length = strlen(strContent);

        if (length < 1)
        {
            printf("wrong format of SECRET KEYS: %d\r\n", length);

            sprintf(strBuf, "%s\r\n", "SETNOK");
            UART3_Transmit((uint8_t *)strBuf, strlen(strBuf));
            return;
        }
        else if (length >= MAX_SECRET_KEY_LEN)
        {
            for (i = 0; i < MAX_SECRET_KEY_LEN; i++)
            {
                pAdvanceInfo->secretKey[i] = strContent[i];
                printf("0x%2x\r\n", pAdvanceInfo->secretKey[i]);
            }
        }
        else
        {
            for (i = 0; i < length; i++)
            {
                pAdvanceInfo->secretKey[i] = strContent[i];
                printf("0x%2x\r\n", pAdvanceInfo->secretKey[i]);
            }
            for (; i < MAX_SECRET_KEY_LEN; i++)
            {
                pAdvanceInfo->secretKey[i] = 'R';
                printf("0x%2x\r\n", pAdvanceInfo->secretKey[i]);
            }
        }
    }
    else
    {

        printf("Unrecognized configset cmd\r\n%s\r\n", strObj);
        sprintf(strBuf, "%s\r\n", "NOK");
        UART3_Transmit((uint8_t *)strBuf, strlen(strBuf));
        return;
    }

    sprintf(strBuf, "%s\r\n", "SETOK");
    UART3_Transmit((uint8_t *)strBuf, strlen(strBuf));
}
void parseConfigRead(char *str, uint8_t len)
{
    char strObj[5];
    char strBuf[64];
    uint8_t i;
    SysInfo_t *pSysInfo;
    Rs485Info_t *pRs485Info;
    AdvanceInfo_t *pAdvanceInfo;
    SecretInfo_t *pSecretInfo = getSecretInfoPointer();

    for (i = 0; i < 4; i++)
    {
        strObj[i] = str[i];
    }
    strObj[i] = '\0';

    if (strcmp(strObj, "VER*") == 0)
    {
        printf("Version:%s\r\n", HADRWIRED_VERSION);
    }
    else if (strcmp(strObj, "MODL") == 0)
    {
    }
    else if (strcmp(strObj, "SYS*") == 0)
    {
        pSysInfo = getSysInfoPointer();
        pAdvanceInfo = getAdvanceInfoPointer();
        pRs485Info = getRs485InfoPointer();
        pAdvanceInfo = getAdvanceInfoPointer();

        printf("read sys info\r\n");

        sprintf(strBuf, "version:%s;model:%s;addr:0x%x%x;chan:0x%x;role:0x%x;baudrate:0x%02x;\
                        parity:0x%02x;stopbits:0x%02x;useEncrypt:%d;packetDelay:0x%x0x%x;",
                pSysInfo->version,
                pSysInfo->model,
                pSysInfo->addrH,
                pSysInfo->addrL,
                pSysInfo->chan,
                pSysInfo->role,
                pRs485Info->baudRate,
                pRs485Info->parity,
                pRs485Info->stopBit,
                pAdvanceInfo->bEncrypt,
                pAdvanceInfo->packetDelayH,
                pAdvanceInfo->packetDelayL);
        UART3_Transmit((uint8_t *)strBuf, strlen(strBuf));

        // sprintf(strBuf, "version%d:%s\r\n", strlen(pSysInfo->version), pSysInfo->version);
        // UART3_Transmit((uint8_t *)strBuf, strlen(strBuf));

        // sprintf(strBuf, "model%d:%s\r\n", strlen(pSysInfo->model), pSysInfo->model);
        // UART3_Transmit((uint8_t *)strBuf, strlen(strBuf));

        // sprintf(strBuf, "addr:0x%x%x\r\n", pSysInfo->addrH, pSysInfo->addrL);
        // UART3_Transmit((uint8_t *)strBuf, strlen(strBuf));
        // sprintf(strBuf, "chan:0x%x\r\n", pSysInfo->chan);
        // UART3_Transmit((uint8_t *)strBuf, strlen(strBuf));
        // sprintf(strBuf, "role:0x%x\r\n", pSysInfo->role);
        // UART3_Transmit((uint8_t *)strBuf, strlen(strBuf));

        // pRs485Info = getRs485InfoPointer();
        // sprintf(strBuf, "Rs485 port:baudrate 0x%02x, parity 0x%02x, stopbits 0x%02x\r\n",
        //         pRs485Info->baudRate,
        //         pRs485Info->parity,
        //         pRs485Info->stopBit);
        // UART3_Transmit((uint8_t *)strBuf, strlen(strBuf));

        // pAdvanceInfo = getAdvanceInfoPointer();
        // sprintf(strBuf, "packet Delay H:0x%x  L: 0x%x\r\n",
        //         pAdvanceInfo->packetDelayH, pAdvanceInfo->packetDelayL);

        // UART3_Transmit((uint8_t *)strBuf, strlen(strBuf));

        // sprintf(strBuf, "useEncrypt:%d\r\n", pAdvanceInfo->bEncrypt);

        // UART3_Transmit((uint8_t *)strBuf, strlen(strBuf));

        // sprintf(strBuf, "secret key is:\r\n");
        // UART3_Transmit((uint8_t *)strBuf, strlen(strBuf));

        sprintf(strBuf, "secretKey:");
        for (i = 0; i < MAX_SECRET_KEY_LEN; i++)
        {
            printf("0x%02x ", pAdvanceInfo->secretKey[i]);
            sprintf(strTemp, "0x%02x", pAdvanceInfo->secretKey[i]);
            //UART3_Transmit((uint8_t *)strBuf, strlen(strBuf));
            strcat(strBuf, strTemp);
        }
        strcat(strBuf,";");
        UART3_Transmit((uint8_t *)strBuf, strlen(strBuf));

        // sprintf(strBuf, "factory key is:\r\n");
        // UART3_Transmit((uint8_t *)strBuf, strlen(strBuf));
        sprintf(strBuf, "factoryKey:");
        for (i = 0; i < MAX_SECRET_KEY_LEN; i++)
        {
            printf("0x%02x ", pSecretInfo->factoryKey[i]);
            sprintf(strTemp, "0x%02x", pSecretInfo->factoryKey[i]);
            strcat(strBuf, strTemp);
        }
        printf("\r\n");
        strcat(strBuf,"\r\n");
        UART3_Transmit((uint8_t *)strBuf, strlen(strBuf));
        // UART3_Transmit("\r\n", 2);
    }
    else
    {

        printf("Unrecognized configread cmd \n%s\r\n", strObj);
        sprintf(strBuf, "%s\r\n", "NOK");
        UART3_Transmit((uint8_t *)strBuf, strlen(strBuf));
    }
}
void parseConfig(char *str, uint8_t len)
{

    char strType[5];
    char strObj[32];
    uint8_t i, j = 0;

    for (i = 0; i < 4; i++)
    {
        strType[i] = str[i];
    }
    strType[i] = '\0';

    for (i = i; i < len; i++)
    {
        strObj[j++] = str[i];
    }
    strObj[j] = '\0';

    if (strcmp(strType, "SET*") == 0)
    {
        parseConfigSet(strObj, strlen(strObj));
    }
    else if (strcmp(strType, "READ") == 0)
    {
        parseConfigRead(strObj, strlen(strObj));
    }
    else
    {
        printf("Unrecognized config cmd:%s\r\n", str);
    }
}
