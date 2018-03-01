#include "newrs485_task.h"
#include "stdio.h"
#include "cmsis_os.h"
#include "globalconfig.h"
#include "usart.h"
#include "message.h"
#include <string.h>
#include "parser.h"
#include "mye2prom.h"
#include "ctrllora.h"
#include "myled.h"

uint8_t mRxState;
uint8_t RX_BUF[BUFFER_MAX_SIZE_RS485];
uint16_t indexRx = 0;

static uint8_t THREAD_RX_BUF[BUFFER_MAX_SIZE_RS485];
static uint8_t indexThreadRx = 0;

extern TaskThread_t mRs485Thread;
extern uint8_t bSlaveReceivedLoraCommand;

void Rs485ConfigByteHandler(uint8_t c)
{
    uint8_t i;

    switch (mRxState)
    {
    case CONFIG_STATE_ZERO:
        if (c == '\r' || c == '\n')
        {
            indexRx = 0;
        }
        else
        {
            RX_BUF[indexRx++] = c;
            mRxState = CONFIG_STATE_BODY;
        }
        break;

    case CONFIG_STATE_LAST_0:
        if (c == '\n')
        {

            // copy buffer to thread buffer
            indexThreadRx = indexRx;
            for (i = 0; i < indexThreadRx; i++)
            {
                THREAD_RX_BUF[i] = RX_BUF[i];
            }
            THREAD_RX_BUF[i] = '\0';
            // signal
            osSignalSet(mRs485Thread.idThread, 0x01);
        }
        mRxState = CONFIG_STATE_ZERO;
        indexRx = 0;

        break;
    case CONFIG_STATE_LAST_1:
        break;
    case CONFIG_STATE_BODY:
        if (c == '\r')
        {
            mRxState = CONFIG_STATE_LAST_0;
        }
        else if (c == '\n')
        {
            indexRx = 0;
            mRxState = CONFIG_STATE_ZERO;
        }
        else
        {
            RX_BUF[indexRx++] = c;
        }

        break;
    default:
        break;
    }
}
void Rs485WorkingByteHandler(uint8_t c)
{
    RX_BUF[indexRx++] = c;
    osSignalSet(mRs485Thread.idThread, 0x01);
}
void Rs485BroadcastingByteHandler(uint8_t c)
{
}
void Rs485WaitingByteHandler(uint8_t c)
{
    // ruff\r\n will make it into the next stage
    switch (mRxState)
    {
    case RX_STATE_ZERO:
        if (c == 'r')
        {
            mRxState = RX_STATE_HEAD;
            RX_BUF[indexRx++] = c;
        }
        else
        {
            indexRx = 0;
        }
        break;
    case RX_STATE_HEAD:

        if (indexRx == BUFFER_MAX_SIZE_RS485)
        {
            indexRx = 0;
            mRxState = RX_STATE_ZERO;
        }
        else if (c == '\r')
        {
            mRxState = RX_STATE_TAIL;
        }
        else
        {
            RX_BUF[indexRx++] = c;
        }

        break;
    case RX_STATE_BODY:
        break;
    case RX_STATE_TAIL:
        if (c == '\n' && RX_BUF[0] == 'r' && RX_BUF[1] == 'u' && RX_BUF[2] == 'f' && RX_BUF[3] == 'f')
        {
            osSignalSet(mRs485Thread.idThread, 0x01);
        }
        else
        {
            indexRx = 0;
            mRxState = RX_STATE_ZERO;
        }
        break;
    default:
        break;
    }
}
void Rs485SyncByteHandler(uint8_t c)
{
}
void Rs485ConfigTask()
{

    osEvent ret;
    mRxState = CONFIG_STATE_ZERO;
    indexRx = 0;

    while (1)
    {
        ret = osSignalWait(0x3, 2000);
        if (ret.status == osEventSignal && ret.value.v == 1)
        {
            printf("%d:%s\r\n", strlen((char *)THREAD_RX_BUF), THREAD_RX_BUF);

            parseConfig((char *)THREAD_RX_BUF, strlen((char *)THREAD_RX_BUF));
        }
        else if (ret.status == osEventSignal && ret.value.v == 2)
        {
            SwitchToWorking();
            break;
        }
    }
}

void Rs485WorkingTask()
{
    uint16_t addr16, i;
    osEvent ret;
    // init

    while (1)
    {
        ret = osSignalWait(0x3, getPacketDelay());

        if (ret.status == osEventSignal && ret.value.v == 1 && mRxState == RS485_STATE_RX_NONE)
        {
            mRxState = RS485_STATE_RX_WAITING;
        }
        else if (ret.status == osEventTimeout && mRxState == RS485_STATE_RX_WAITING)
        {
            printf("\nGet data from RS485, len:%d\r\n", indexRx);

            for (i = 0; i < indexRx; i++)
            {
                printf("0x%02x ", RX_BUF[i]);
            }
            printf("\r\n");

            if (indexRx <= 2)
            {
                printf("too few bytes\r\n");
            }
            else if (GetRole() == MASTER ||
                     (GetRole() == SLAVE && bSlaveReceivedLoraCommand == 1))
            {
                addr16 = getAddress();
                printf("Print out addr:%x\r\n", addr16);
                if (isUseEncrypt() == 1)
                {
                    SendOutLoraData(1, addr16, RX_BUF, indexRx);
                }
                else
                {
                    SendOutLoraData(0, addr16, RX_BUF, indexRx);
                }

                bSlaveReceivedLoraCommand = 0;
            }
            else if (GetRole() == SLAVE)
            {
                printf("Can not send before received from master\r\n");
            }

            indexRx = 0;

            mRxState = RS485_STATE_RX_NONE;

            // toggle led2
            // FlashLED2();
        }
        else if (ret.status == osEventTimeout && mRxState == RS485_STATE_RX_NONE)
        {
        }

        if (GetStateContext().type != 1)
        {
            printf("Get out of rs485 working task\r\n");
            break;
        }
    }
}
void Rs485BroadcastingMaster()
{
    uint16_t addr16 = getAddress();
    //uint8_t buf[] = {99};
    BroadcastingMessage_t msg;
    Rs485Info_t *pRs485 = getRs485InfoPointer();
    int i;

    msg.magicNum = MAGIC_NUM;
    msg.channel = getChannel();

    printf("Send out factory key");
    if (isUseEncrypt() == 1)
    {
        getEncryptedFactoryKey(msg.factoryKey);
    }
    else
    {
        getFactoryKey(msg.factoryKey);
    }
    for (i = 0; i < MAX_SECRET_KEY_LEN; i++)
    {
        printf("0x%02x ", msg.factoryKey[i]);
    }
    printf("\r\n");

    msg.baudRate = pRs485->baudRate;
    msg.parity = pRs485->parity;
    msg.stopBit = pRs485->stopBit;

    while (1)
    {
        // send out lora message every 200 ms
        osDelay(1000);
        printf("485-broadcasting-master\r\n");
        SendOutLoraData(0, addr16, (uint8_t *)(&msg), BROADCASTINGMESSAGE_SIZE);
    }
}
void Rs485BroadcastingSlave()
{
    while (1)
    {
        osDelay(2000);
        //printf("485-broadcasting-slave\r\n");
    }
}
void Rs485BroadcastingTask()
{
    if (GetRole() == SLAVE)
    {
        Rs485BroadcastingSlave();
    }
    else
    {
        Rs485BroadcastingMaster();
    }
}
void Rs485WaitingTask()
{
    mRxState = RX_STATE_ZERO;
    indexRx = 0;
    // Triger uart3 receive
    UART3_Receive();
    UART2_Receive();

    printf("In Waiting state\r\n");

    osEvent ret = osSignalWait(0x3, 10000);

    if (ret.status == osEventSignal)
    {
        printf("Should go to Config state\r\n");

        UART3_Transmit("OK\r\n", 4);

        // otherwise it will switch to the config state
        SwitchToConfig();
    }
    else if (ret.status == osEventTimeout)
    {
        printf("timeout\r\n");
        SwitchToWorking();
    }
}
void Rs485SyncTask()
{
    osDelay(2000);
    printf("\r\nrs485-sync\r\n");
}