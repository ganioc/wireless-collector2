#pragma once

#include "main.h"

#define LORA_STATE_RX_WAITING    0x01
#define LORA_STATE_RX_NONE       0x02


void LoraConfigByteHandler(uint8_t c);
void LoraWorkingByteHandler(uint8_t c);
void LoraBroadcastingByteHandler(uint8_t c);
void LoraWaitingByteHandler(uint8_t c);
void LoraSyncByteHandler(uint8_t c);

void LoraConfigTask();

void LoraWorkingTask();

void LoraBroadcastingTask();

void LoraWaitingTask();

void ConfigLora();

void LoraSyncTask();
