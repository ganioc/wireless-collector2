#pragma once
#include "main.h"


// int encrypt(char buf[], int len, uint8_t * secret);
// int decrypt(char buf[], int len, uint8_t * secret);

int encryptSecret(char buf[], int len);
int encryptFactory(char buf[], int len);
int decryptSecret(char buf[], int len);
int decryptFactory(char buf[], int len);

void testEncrypt();
void testEncrypt2();
void Encrypt_Init();
    
