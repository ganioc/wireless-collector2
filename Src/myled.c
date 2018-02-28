#include "myled.h"
#include "cmsis_os.h"
#include "globalconfig.h"

uint16_t mDelayPeriod = DELAY_QUICK;
uint16_t mLED1Counter = 0;
uint16_t mLED2Counter = 0;

void LED_On(uint16_t pin)
{
    HAL_GPIO_WritePin(LED_PORT, pin, GPIO_PIN_RESET);
}

void LED_Off(uint16_t pin)
{
    HAL_GPIO_WritePin(LED_PORT, pin, GPIO_PIN_SET);
}

void LED_Toggle(uint16_t pin)
{
    HAL_GPIO_TogglePin(LED_PORT, pin);
}

void SetLED1Quick()
{
    mDelayPeriod = DELAY_QUICK;
}
void SetLED1Slow()
{
    mDelayPeriod = DELAY_SLOW;
}
void SetLED1Normal()
{
    mDelayPeriod = DELAY_NORMAL;
}

void FlashLED2()
{

    LED2_Toggle();
    osDelay(DELAY_FLASH);
    LED2_Toggle();
    //            osDelay( DELAY_FLASH);
    //            LED2_Toggle();
    //            osDelay( DELAY_FLASH);
    //            LED2_Toggle();
}

uint8_t bGetDefaultKey()
{
    GPIO_PinState state = HAL_GPIO_ReadPin(DEFAULT_KEY_PORT, DEFAULT_KEY_PIN);

    if (state == GPIO_PIN_SET)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}
uint8_t bGetAuxKey()
{
    GPIO_PinState state = HAL_GPIO_ReadPin(AUX_KEY_PORT, AUX_KEY_PIN);

    if (state == GPIO_PIN_SET)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}
void RunLED2MasterPattern(uint16_t t)
{
    mLED2Counter++;

    if (mLED2Counter == 2)
    {
        LED2_On();
    }
    else if (mLED2Counter == 20)
    {
        LED2_Off();
        mLED2Counter = 0;
    }
}
void RunLED2SlavePattern(uint16_t t)
{
    mLED2Counter++;

    if (mLED2Counter == 18)
    {
        LED2_On();
    }
    else if (mLED2Counter == 20)
    {
        LED2_Off();
        mLED2Counter = 0;
    }
}
void RunLED1ConfigPattern(uint16_t t)
{
    mLED1Counter++;

    if (mLED1Counter == 9)
    {
        LED1_On();
    }
    else if (mLED1Counter == 10)
    {
        LED1_Off();
        mLED1Counter = 0;
    }
}

void RunLED2ConfigPattern(uint16_t t)
{
    if (GetRole() == SLAVE)
    {
        RunLED2SlavePattern(t);
    }
    else
    {
        RunLED2MasterPattern(t);
    }
}

void RunLED1WorkingPattern(uint16_t t)
{
    mLED1Counter++;

    if (mLED1Counter == 5)
    {
        LED1_On();
    }
    else if (mLED1Counter == 10)
    {
        LED1_Off();
        mLED1Counter = 0;
    }
}

void RunLED2WorkingPattern(uint16_t t)
{
    if (GetRole() == SLAVE)
    {
        RunLED2SlavePattern(t);
    }
    else
    {
        RunLED2MasterPattern(t);
    }
}

void RunLED1BroadcastingPattern(uint16_t t)
{
    mLED1Counter++;

    if (mLED1Counter == 5)
    {
        LED1_On();
    }
    else if (mLED1Counter == 30)
    {
        LED1_Off();
        mLED1Counter = 0;
    }
}

void RunLED2BroadcastingPattern(uint16_t t)
{
    if (GetRole() == SLAVE)
    {
        RunLED2SlavePattern(t);
    }
    else
    {
        RunLED2MasterPattern(t);
    }
}

void RunLED1WaitingPattern(uint16_t t)
{
    mLED1Counter++;

    if (mLED1Counter == 1)
    {
        LED1_Toggle();
        mLED1Counter = 0;
    }
}

void RunLED2WaitingPattern(uint16_t t)
{
    if (GetRole() == SLAVE)
    {
        RunLED2SlavePattern(t);
    }
    else
    {
        RunLED2MasterPattern(t);
    }
}
void RunLED1SyncPattern(uint16_t t){
    mLED1Counter++;

    if (mLED1Counter == 1)
    {
        LED1_Toggle();
        mLED1Counter = 0;
    }
}
void RunLED2SyncPattern(uint16_t t){
    mLED2Counter++;

    if (mLED2Counter == 1)
    {
        LED2_Toggle();
        mLED2Counter = 0;
    }
}
void ResetLEDCounter()
{
    mLED1Counter = 0;
    mLED2Counter = 0;
}
