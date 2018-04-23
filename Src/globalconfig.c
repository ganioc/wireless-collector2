#include "globalconfig.h"
#include "newThread_lora.h"
#include "mye2prom.h"
#include "newThread_rs485.h"
#include "newRs485_task.h"
#include "newLora_task.h"
#include "myled.h"
#include "cmsis_os.h"
#include "ctrllora.h"
#include "thread_crypto.h"

StateType globalState;
RoleType globalRole;

uint8_t bSlaveReceivedLoraCommand = 0;

extern uint8_t mRxState;
extern uint16_t indexRx;
extern uint8_t mLoraState;
extern uint16_t indexRxLora;
extern uint16_t indexRxForRs485;

void (*handlerByteRs485Cb)(uint8_t);
void (*handlerByteLoraCb)(uint8_t);

void ConfigStateThreadInit()
{
    NewThreadLoraInit();

    NewThreadRs485Init();

    CryptoThreadInit();
}

StateContext_t stateContexts[] = {
    {"config",
     0,
     LoraConfigByteHandler,
     Rs485ConfigByteHandler,
     LoraConfigTask,
     Rs485ConfigTask,
     RunLED1ConfigPattern,
     RunLED2ConfigPattern},
    {"working",
     1,
     LoraWorkingByteHandler,
     Rs485WorkingByteHandler,
     LoraWorkingTask,
     Rs485WorkingTask,
     RunLED1WorkingPattern,
     RunLED2WorkingPattern},
    {"broadcasting",
     2,
     LoraBroadcastingByteHandler,
     Rs485BroadcastingByteHandler,
     LoraBroadcastingTask,
     Rs485BroadcastingTask,
     RunLED1BroadcastingPattern,
     RunLED2BroadcastingPattern},
    {"waiting",
     3,
     LoraWaitingByteHandler,
     Rs485WaitingByteHandler,
     LoraWaitingTask,
     Rs485WaitingTask,
     RunLED1WaitingPattern,
     RunLED2WaitingPattern},
    {"sync",
     4,
     LoraSyncByteHandler,
     Rs485SyncByteHandler,
     LoraSyncTask,
     Rs485SyncTask,
     RunLED1SyncPattern,
     RunLED2SyncPattern},
};

void SwitchState(StateType s)
{
    globalState = s;
    // uart byte handler callback
    handlerByteLoraCb = GetStateContext().loraHandler;
    handlerByteRs485Cb = GetStateContext().rs485Handler;

    ResetLEDCounter();
}
void SwitchToConfig()
{
    SwitchState(STATE_TYPE_CONFIG);
    printf("Switch to Config state\r\n");

}
void SwitchToWorking()
{
    
    printf("Switch to Working state, \r\n");
    // osDelay(3000);
    
    SetLoraSettingMode();
    while (bGetAuxKey() == 0)
    {
        osDelay(5);
    }
    osDelay(5);
    ConfigLora();
    while (bGetAuxKey() == 0)
    {
        osDelay(5);
    }
    osDelay(5);
    SetLoraWorkingMode();
    osDelay(5);

    SwitchState(STATE_TYPE_WORKING);
    mRxState = RS485_STATE_RX_NONE;
    indexRx = 0;
    mLoraState = LORA_STATE_RX_NONE;
    indexRxLora = 0;
    indexRxForRs485 = 0;
    SwitchState(STATE_TYPE_WORKING);
}
void SwitchToBroadcasting()
{

    SwitchState(STATE_TYPE_BROADCASTING);
    mLoraState = LORA_STATE_RX_NONE;
    indexRxLora = 0;

    printf("Switch to Broadcasting state\r\n");
}
void SwitchToWaiting()
{
    SwitchState(STATE_TYPE_WAITING);
    printf("Switch to Waiting state\r\n");
}
void SwitchToSync(){
    SwitchState(STATE_TYPE_SYNC);
    mLoraState = LORA_STATE_RX_NONE;
    indexRxLora = 0;
    printf("Switch to Sync state\r\n");
}
StateContext_t GetStateContext()
{
    return stateContexts[globalState];
}
RoleType GetRole()
{
    return globalRole;
}

void GlobalConfig_Init()
{
    printf("\r\nGlobalConfig\r\n");
    // default state
    SwitchState(STATE_TYPE_WAITING);

    // decide Master or Slave
    if (getSysInfoRole() == ROLE_MASTER)
    {
        globalRole = MASTER;
        printf("Its a master\r\n");
    }
    else
    {
        globalRole = SLAVE;
        printf("Its a slave\r\n");
    }

    printf("\r\nCurrent state is %d\r\n", globalState);
    printf("%d %s\r\n", GetStateContext().type,
           GetStateContext().name);
    ConfigStateThreadInit();
}
