#include "newThread_lora.h"
#include "message.h"
#include "stdio.h"
#include "cmsis_os.h"
#include "globalconfig.h"

TaskThread_t mLoraThread;

static void TaskLoop(void const * argument){
    //osEvent ret;
    osDelay(300);
    
    printf("\r\nnew loraThread taskloop started\r\n");

    while(1){
        GetStateContext().loraTaskLoop();  
    }
}

void NewThreadLoraInit(){
    osThreadDef(lorathread, TaskLoop, osPriorityHigh, 0, 128);
    mLoraThread.idThread = osThreadCreate(osThread(lorathread), NULL);

    if(mLoraThread.idThread == NULL)
    {

        printf("new lorathread create fail\r\n");
    }
    else
    {
        printf("new lorathread create OK\r\n");
    }
}