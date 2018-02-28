#include "newThread_rs485.h"
#include "message.h"
#include "stdio.h"
#include "cmsis_os.h"
#include "globalconfig.h"

TaskThread_t mRs485Thread;


static void TaskLoop(void const * argument)
{

    //osEvent ret;

    osDelay(500);

    printf("\r\nnew mRs485Thread taskloop started\r\n");

    // LocalInit();

    while(1){
        GetStateContext().rs485TaskLoop();  
    }
}


void NewThreadRs485Init()
{

    osThreadDef(rs485thread, TaskLoop, osPriorityHigh, 0, 256);
    mRs485Thread.idThread = osThreadCreate(osThread(rs485thread), NULL);

    if(mRs485Thread.idThread == NULL)
    {

        printf("new rs485thread create fail\r\n");
    }
    else
    {
        printf("new rs485thread create OK\r\n");
    }
}
