#pragma once
#include "main.h"


#define RX_STATE_HEAD  0x01
#define RX_STATE_BODY  0x02
#define RX_STATE_TAIL   0x03
#define RX_STATE_ZERO  0x04

#define CONFIG_STATE_ZERO     0x01
#define CONFIG_STATE_LAST_1 0x02
#define CONFIG_STATE_LAST_0 0x03
#define CONFIG_STATE_BODY     0x04

#define BUFFER_MAX_SIZE_RS485 300

#define RS485_STATE_RX_WAITING    0x01
#define RS485_STATE_RX_NONE       0x02

void Rs485ConfigByteHandler(uint8_t c);
void Rs485WorkingByteHandler(uint8_t c);
void Rs485BroadcastingByteHandler(uint8_t c);
void Rs485WaitingByteHandler(uint8_t c);
void Rs485SyncByteHandler(uint8_t c);

void Rs485ConfigTask();
void Rs485WorkingTask();
void Rs485BroadcastingTask();
void Rs485WaitingTask();
void Rs485SyncTask();

