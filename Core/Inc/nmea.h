#ifndef NMEA_H_
#define NMEA_H_

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "stm32f1xx_hal.h"

#define NMEA0183_MAX_LENGTH 82

#define VALID 0
#define INVALID 1

typedef struct {
	int hour, minute, latdeg, longdeg, day, month, year, checksum;
	int Hlatmin, Hlongmin, Llatmin, Llongmin;
	float second, speed, tracktrue, magvar;
	char valid, latdir, longdir, magvardir, mode;
} GPRMC_t; //Para almacenar datos de GPRMC, después lo movemos

uint8_t ParseGPRMC(uint8_t *, GPRMC_t *);
uint8_t CalculateChecksum(uint8_t *);
void BuildGPSCoord(int16_t, uint8_t, uint32_t, uint8_t, uint8_t*);
char *strtok_new(char*, const char*);

#endif /* NMEA_H_ */
