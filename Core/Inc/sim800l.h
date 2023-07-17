#ifndef SIM800L_H_
#define SIM800L_H_

#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include "stm32f1xx_hal.h"

#define BUF_SIZE 100

#define SUCCESS 0
#define FATAL_ERROR 1
#define RETRY 2
#define WEB_RETRY 3
#define SMS_RETRY 4
#define CONTINUE 5
#define CONTINUE_AND_RETRY 6
#define NO_READ 7

#define MAX_RETRY 3

#define FOUND 0
#define NOT_FOUND 1

uint8_t SIM800L_SendSMS(uint8_t *);
uint8_t SIM800L_WEBUpdate(uint8_t *);
uint8_t SIM800L_BatteryCheck(uint16_t *);
void signalQuality(void);
uint8_t SIM800L_CheckResponse(uint8_t *, uint8_t *, uint8_t *, uint8_t *);
void SIM800L_Reset();
void SIM800L_Sleep();
void SIM800L_Wakeup();

#endif /* SIM800L_H_ */
