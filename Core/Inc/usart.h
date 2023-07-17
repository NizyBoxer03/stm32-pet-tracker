#ifndef USART_H_
#define USART_H_

#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include "stm32f1xx_hal.h"

#define SUCCESS 0
#define ERROR 1

uint8_t GPS_RX(uint8_t *);
uint8_t SIM_RX();

#endif /* USART_H_ */