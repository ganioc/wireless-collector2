/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * Copyright (c) 2017 STMicroelectronics International N.V. 
  * All rights reserved.
  *
  * Redistribution and use in source and binary forms, with or without 
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice, 
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other 
  *    contributors to this software may be used to endorse or promote products 
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this 
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under 
  *    this license is void and will automatically terminate your rights under 
  *    this license. 
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS" 
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT 
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT 
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_os.h"

/* USER CODE BEGIN Includes */
#include "myled.h"
#include "thread_rs485.h"
#include "thread_lora.h"
#include "mye2prom.h"
#include "thread_crypto.h"
#include "globalconfig.h"
/* USER CODE END Includes */

/* Variables -----------------------------------------------------------------*/
osThreadId defaultTaskHandle;

/* USER CODE BEGIN Variables */
extern uint16_t mDelayPeriod;
extern uint32_t wwdg_life_counter;

uint8_t setDefaultCounter = 0;
uint16_t keyDownCounter = 0;

/* USER CODE END Variables */

/* Function prototypes -------------------------------------------------------*/
void StartDefaultTask(void const *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

/* Hook prototypes */

/* Init FreeRTOS */

void MX_FREERTOS_Init(void)
{
    /* USER CODE BEGIN Init */

    /* USER CODE END Init */

    /* USER CODE BEGIN RTOS_MUTEX */
    /* add mutexes, ... */
    /* USER CODE END RTOS_MUTEX */

    /* USER CODE BEGIN RTOS_SEMAPHORES */
    /* add semaphores, ... */
    /* USER CODE END RTOS_SEMAPHORES */

    /* USER CODE BEGIN RTOS_TIMERS */
    /* start timers, add new ones, ... */
    /* USER CODE END RTOS_TIMERS */

    /* Create the thread(s) */
    /* definition and creation of defaultTask */
    osThreadDef(defaultTask, StartDefaultTask, osPriorityNormal, 0, 128);
    defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

    /* USER CODE BEGIN RTOS_THREADS */
    /* add threads, ... */
    //Rs485ThreadInit();
    //LoraThreadInit();
    //CryptoThreadInit();
    GlobalConfig_Init();
    /* USER CODE END RTOS_THREADS */

    /* USER CODE BEGIN RTOS_QUEUES */
    /* add queues, ... */
    /* USER CODE END RTOS_QUEUES */
}
void CheckManualSetting()
{
    if (bGetDefaultKey() == 1 && setDefaultCounter >= TO_BROADCASTING_TIME && setDefaultCounter < TO_BROADCASTING_TIME + 3)
    {
        // set to broadcasting mode
        // setDefaultCounter++;
        printf("Switch to Broadcasting mode\r\n");

        // osDelay(1000);
        // wwdg_life_counter = 0;
        // osDelay(1000);
        // wwdg_life_counter = 0;
        // osDelay(1000);
        // wwdg_life_counter = 0;
        // osDelay(1000);
        // wwdg_life_counter = 0;
        // osDelay(1000);
        // wwdg_life_counter = 0;

        if (GetStateContext().type == 1)
        {
            // SwitchToBroadcasting();
            printf("Cant switch to Broadcasting()\r\n");
        }

        setDefaultCounter = 0;
    }
    else if (setDefaultCounter >= TO_BROADCASTING_TIME && setDefaultCounter < TO_BROADCASTING_TIME + 4)
    {
        printf("approach Into Broadcasting mode\r\n");

        LED2_Toggle();
        osDelay(100);
        wwdg_life_counter = 0;
        LED2_Toggle();
        osDelay(100);
        wwdg_life_counter = 0;
        LED2_Toggle();
        osDelay(100);
        wwdg_life_counter = 0;
        LED2_Toggle();
        osDelay(100);
        wwdg_life_counter = 0;
        LED2_Toggle();
        osDelay(100);
        wwdg_life_counter = 0;
        LED2_Toggle();
        osDelay(100);
        wwdg_life_counter = 0;
        LED2_Toggle();
        osDelay(100);
        wwdg_life_counter = 0;
        LED2_Toggle();
        osDelay(100);
        wwdg_life_counter = 0;
        LED2_Toggle();
        osDelay(100);
        wwdg_life_counter = 0;                

        setDefaultCounter++;
    }
    else if (bGetDefaultKey() == 1 && setDefaultCounter >= TO_DEFAULT_TIME && setDefaultCounter < TO_DEFAULT_TIME + 4)
    {
        // set to default mode
        // reset the E2PROM
        ResetToDefaultE2Prom();

        printf("Restart the board now, dont hold it now ...\r\n");

        // then restart the board
        HAL_NVIC_SystemReset();
    }
    else if (setDefaultCounter >= TO_DEFAULT_TIME && setDefaultCounter < TO_DEFAULT_TIME + 4)
    {
        printf("approach Into SetDefault mode\r\n");

        LED1_Toggle();
        osDelay(100);
        wwdg_life_counter = 0;
        LED1_Toggle();
        osDelay(100);
        wwdg_life_counter = 0;
        LED1_Toggle();
        osDelay(100);
        wwdg_life_counter = 0;
        LED1_Toggle();
        osDelay(100);
        wwdg_life_counter = 0;
        LED1_Toggle();
        osDelay(100);
        wwdg_life_counter = 0;
        LED1_Toggle();
        osDelay(100);
        wwdg_life_counter = 0;
        LED1_Toggle();
        osDelay(100);
        wwdg_life_counter = 0;
        LED1_Toggle();
        osDelay(100);
        wwdg_life_counter = 0;

        setDefaultCounter++;
    }
    else if (bGetDefaultKey() == 1 && setDefaultCounter < TO_BROADCASTING_TIME)
    {
        setDefaultCounter = 0;
    }
    else if (bGetDefaultKey() == 0)
    {
        setDefaultCounter++;
    }
    else
    {
        setDefaultCounter = 0;
    }
    //printf("defaultCounter:%d\r\n", setDefaultCounter);
}
void CheckKeyDown()
{
    keyDownCounter++;

    if (keyDownCounter >= DELAY_TICK_NUM)
    {
        keyDownCounter = 0;
        CheckManualSetting();
    }
}
/* StartDefaultTask function */
void StartDefaultTask(void const *argument)
{

    /* USER CODE BEGIN StartDefaultTask */
    /* Infinite loop */
    for (;;)
    {
        osDelay(DELAY_TICK);

        wwdg_life_counter = 0;

        GetStateContext().led1Handler(DELAY_TICK);
        GetStateContext().led2Handler(DELAY_TICK);
        CheckKeyDown();
    }
    /* USER CODE END StartDefaultTask */
}

/* USER CODE BEGIN Application */

/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
