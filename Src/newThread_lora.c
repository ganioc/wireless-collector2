#include "newThread_lora.h"
#include "message.h"
#include "stdio.h"
#include "cmsis_os.h"
#include "globalconfig.h"
#include "myled.h"

TaskThread_t mLoraThread;

static void TaskLoop(void const * argument){
    //osEvent ret;
    osDelay(300);
    
    printf("\r\nnew loraThread taskloop started\r\n");
    // enable Lora module power supply after 3 seconds
    osDelay(3000);
    EnableLora();

    while(1){
        GetStateContext().loraTaskLoop();  
    }
}

void NewThreadLoraInit(){
    osThreadDef(lorathread, TaskLoop, osPriorityHigh, 0, 256);
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