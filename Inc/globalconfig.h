#pragma once

#include "main.h"

typedef enum {
    STATE_TYPE_CONFIG = 0,
    STATE_TYPE_WORKING = 1,
    STATE_TYPE_BROADCASTING = 2,
    STATE_TYPE_WAITING = 3,
    STATE_TYPE_SYNC = 4,
} StateType;

typedef enum {
    SLAVE = 0,
    MASTER = 1
} RoleType;

typedef struct
{
    char name[20];
    uint8_t type;
    void (*loraHandler)(uint8_t c);
    void (*rs485Handler)(uint8_t c);
    void (*loraTaskLoop)();
    void (*rs485TaskLoop)();
    void (*led1Handler)(uint16_t t);
    void (*led2Handler)(uint16_t t);

} StateContext_t;

void GlobalConfig_Init();
StateContext_t GetStateContext();
RoleType GetRole();
void SwitchToConfig();
void SwitchToWorking();
void SwitchToBroadcasting();
void SwitchToWaiting();
void SwitchToSync();
