#ifndef SIM800L_H_
#define SIM800L_H_

#include <stdint.h>
#include <string.h>
#include "stm32f1xx_hal.h"

void SIM800L_Init();
void SIM800L_CheckStatus();
void SIM800L_SMSConfig();
void SIM800L_SendMessage();

#endif /* SIM800L_H_ */