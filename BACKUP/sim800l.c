#include "sim800l.h"

extern UART_HandleTypeDef huart3;

extern uint8_t SIM800L_response[BUF_SIZE];

//sprintf(URL, "AT+HTTPPARA=\"URL\",\"http://186.13.143.39:80/gpsdata.php\?lat=%f\&lng=%f\"\n\r", ConvertToDecimalDegrees(GPS.latdeg, GPS.latmin, GPS.latdir), ConvertToDecimalDegrees(GPS.longdeg, GPS.longmin, GPS.longdir));

extern bool simResponseReady;

const char *SMS_PACK[] = {
    "AT\n\r",
    "AT+CPIN?\n\r",
    "AT+CFUN=1\n\r",
    "AT+CMGF=1\n\r",
    "AT+CSCA=\"+541151740055\"\n\r",
    "AT+CMGS=\"+5491134285648\"\n\r",
    "SMS\n\r" //\0x1A
};

const char *HTTP_PACK[] = {
    "AT\n\r",
    "AT+CPIN?\n\r",
    "AT+CFUN=1\n\r",
    "AT+CSTT=\"datos.personal.com\"\n\r",
	"AT+CIICR\n\r",
	"AT+CIFSR\n\r",
    "AT+SAPBR=3,1,\"Contype\",\"GPRS\"\n\r",
    "AT+SAPBR=3,1,\"APN\",\"datos.personal.com\"\n\r",
    "AT+SAPBR=1,1\n\r",
    "AT+HTTPINIT\n\r",
	"AT+HTTPSSL=0\n\r",
    "AT+HTTPPARA=\"CID\",1\n\r",
    "WEBSERVER\n\r",
	"AT+HTTPACTION=0\n\r",
    "AT+HTTPTERM\n\r",
	"AT+SAPBR=0,1\n\r",
	"AT+CIPSHUT\n\r"
};

const char *FATAL_PACK[] = {
    "AT+SAPBR=0,1\n\r",
	"AT+CIPSHUT\n\r"
};

const char BATTERY[] = "AT+CBC\n\r";

void signalQuality(void) {
    HAL_UART_Transmit_DMA(&huart3, "AT+CSQ\n\r", strlen("AT+CSQ\n\r"));
    HAL_Delay(1000);
    HAL_UART_Transmit_DMA(&huart3, "AT+SAPBR=0,1\n\r", strlen("AT+SAPBR=0,1\n\r"));
    HAL_Delay(1000);
    HAL_UART_Transmit_DMA(&huart3, "AT+CIPSHUT\n\r", strlen("AT+CIPSHUT\n\r"));
    HAL_Delay(1000);
}

uint8_t SIM800L_BatteryCheck(uint16_t *voltage)
{
    static uint8_t retryCounter = 0;
    static bool startTransmission = true;
    static bool secondResponse = false;
    static bool dataRead = false;

    static uint8_t *batteryVoltage;
    uint8_t idx = 0;

    uint8_t retValue;

    if(startTransmission) {
        HAL_UART_Transmit_DMA(&huart3, BATTERY, strlen((char*)BATTERY));
        startTransmission = false;
    }
    if(simResponseReady) {
        if(!secondResponse) {
            if(SIM800L_CheckResponse(SIM800L_response, NULL, "+CBC: ", batteryVoltage) == FOUND) {
                *voltage = 0;

                secondResponse = true;
                retValue = CONTINUE;
            } else {
                retValue = RETRY; 
            }
        } else {
            if(SIM800L_CheckResponse(SIM800L_response, "OK", NULL, NULL) == FOUND) {
                while (batteryVoltage[idx] != ',') idx++; // 1st ,
                idx++;
                while (batteryVoltage[idx] != ',') idx++; // After time ,
                idx++;

                *voltage = (batteryVoltage[idx] - '0') * 1000 + (batteryVoltage[idx + 1] - '0') * 100 + (batteryVoltage[idx + 2] - '0') * 10 + (batteryVoltage[idx + 3] - '0');

                secondResponse = false;
                retValue = SUCCESS;
            } else {
                retValue = RETRY; 
            }
        }   

        dataRead = true;
        simResponseReady = false;
    } else {
        dataRead = false;
        retValue = NO_READ;
    }

    if(retValue == RETRY || retValue == WEB_RETRY) {
        dataRead = false;
        secondResponse = false;
        startTransmission = true;
        retryCounter++;

        HAL_Delay(250);
    } 

    if(retValue == CONTINUE) {
        retryCounter = 0;
    }

    if(retryCounter == MAX_RETRY) {
        retValue = FATAL_ERROR;
    }
    if(retValue == SUCCESS || retValue == FATAL_ERROR) {
        retryCounter = 0;
        dataRead = false;
        startTransmission = true;
        secondResponse = false;
    }
    return retValue;
}

uint8_t SIM800L_SendSMS(uint8_t *sms)
{
    static uint8_t i = 0;
    static uint8_t retryCounter = 0;
    static bool startTransmission = true;
    static bool secondResponse = false;
    static bool dataRead = false;

    uint8_t retValue = 255;

    if(startTransmission) {
        if(i == 6) {
            HAL_UART_Transmit_DMA(&huart3, sms, strlen((char*) sms));
        } else {
            HAL_UART_Transmit_DMA(&huart3, SMS_PACK[i], strlen((char*)SMS_PACK[i]));
        }
        startTransmission = false;
    }
    if(simResponseReady) {
        if(i == 1) {
            if(!secondResponse) {
                if(SIM800L_CheckResponse(SIM800L_response, "READY", NULL, NULL) == FOUND) {
                    secondResponse = true;
                    retValue = CONTINUE;
                } else {
                    retValue = RETRY; 
                }
            } else {
                if(SIM800L_CheckResponse(SIM800L_response, "OK", NULL, NULL) == FOUND) {
                    secondResponse = false;
                    retValue = CONTINUE;
                } else {
                    retValue = RETRY; 
                }
            }
        } else if(i == 6) {
            if(!secondResponse) {
                if(SIM800L_response[0] != NULL) {
                    secondResponse = true;
                    retValue = CONTINUE;
                } else {
                    retValue = SMS_RETRY; 
                }
            } else {
                if(SIM800L_CheckResponse(SIM800L_response, "OK", NULL, NULL) == FOUND) {
                    secondResponse = false;
                    retValue = SUCCESS;
                } else {
                    retValue = SMS_RETRY; 
                }
            }
        } else {
            if(SIM800L_CheckResponse(SIM800L_response, "OK", NULL, NULL) == FOUND) {
                retValue = CONTINUE;
            } else {
                retValue = RETRY; 
            }
        }

        dataRead = true;
        simResponseReady = false;
    } else {
        dataRead = false;
        retValue = NO_READ;
    }

    if(retValue == RETRY || retValue == WEB_RETRY) {
        dataRead = false;
        secondResponse = false;
        startTransmission = true;
        retryCounter++;

        HAL_Delay(250);
    } else {
        if(!secondResponse && dataRead) {
            dataRead = false;
            startTransmission = true;
            i++;
        }
    }

    if(retValue == CONTINUE) {
        retryCounter = 0;
    }

    if(retryCounter == MAX_RETRY) {
        retValue = FATAL_ERROR;
    }
    if(retValue == SUCCESS || retValue == FATAL_ERROR) {
        if(retValue == SUCCESS) i = 0;
        retryCounter = 0;
        dataRead = false;
        startTransmission = true;
        secondResponse = false;
    }
    return retValue;
}

uint8_t SIM800L_MapUpdate(uint8_t *url)
{
    static uint8_t i = 0;
    static uint8_t retryCounter = 0;
    static bool startTransmission = true;
    static bool secondResponse = false;
    static bool dataRead = false;

    uint8_t retValue;

    //signalQuality();
    //Emergency fix

        for (uint8_t i = 0; i < 17; i++) {
        	if(i == 5) HAL_Delay(5000);
        	if (i == 12)
        	{
        		HAL_UART_Transmit_DMA(&huart3, url, strlen((char*) url));
        		HAL_Delay(1000);
        		continue;
        	}
        	if(i == 14 || i == 15) HAL_Delay(5000);
            HAL_UART_Transmit_DMA(&huart3, HTTP_PACK[i], strlen((char*)HTTP_PACK[i]));
            HAL_Delay(1000);
        }
/*	//Emergency fix
    if(startTransmission) {
        if(i == 12) {
            HAL_UART_Transmit_DMA(&huart3, url, strlen((char*) url));
            HAL_Delay(250);
        } else {
            HAL_UART_Transmit_DMA(&huart3, HTTP_PACK[i], strlen((char*)HTTP_PACK[i]));
            HAL_Delay(250);
        }
        startTransmission = false;
    }
    if(simResponseReady) {
        if(i == 1) {
            if(!secondResponse) {
                if(SIM800L_CheckResponse(SIM800L_response, "READY", NULL, NULL) == FOUND) {
                    secondResponse = true;
                    retValue = CONTINUE;
                } else {
                    retValue = RETRY;
                }
            } else {
                if(SIM800L_CheckResponse(SIM800L_response, "OK", NULL, NULL) == FOUND) {
                    secondResponse = false;
                    retValue = CONTINUE;
                } else {
                    retValue = RETRY;
                }
            }
        } else if(i == 5) {
            if(SIM800L_response[0] != NULL) {
                retValue = CONTINUE;
            } else {
                retValue = RETRY;
            }
        } else if(i == 13) {
            if(!secondResponse) {
                if(SIM800L_CheckResponse(SIM800L_response, "OK", NULL, NULL) == FOUND) {
                    secondResponse = true;
                    retValue = CONTINUE;
                } else {
                    retValue = WEB_RETRY;
                }
            } else {
                if(SIM800L_CheckResponse(SIM800L_response, "200", NULL, NULL) == FOUND) {
                    secondResponse = false;
                    retValue = CONTINUE;
                } else {
                    retValue = WEB_RETRY;
                }
            }
        } else {
            if(SIM800L_CheckResponse(SIM800L_response, "OK", NULL, NULL) == FOUND) {
                if(i == 16) retValue = SUCCESS;
                else retValue = CONTINUE;
            } else {
                retValue = RETRY;
            }
        }

        dataRead = true;
        simResponseReady = false;
    } else {
        dataRead = false; //herein lies part of the problem: always no read
        retValue = NO_READ;
    }

    if(retValue == RETRY || retValue == WEB_RETRY) {
        dataRead = false;
        secondResponse = false;
        startTransmission = true;
        retryCounter++;

        HAL_Delay(250);
    } else {
        if(!secondResponse && dataRead) {
            dataRead = false;
            startTransmission = true;
            i++;
        }
    }

    if(retValue == CONTINUE) {
        retryCounter = 0;
    }

    if(retryCounter == MAX_RETRY) {
        retValue = FATAL_ERROR;
    }
    if(retValue == SUCCESS || retValue == FATAL_ERROR) {
        if(retValue == SUCCESS) i = 0;
        retryCounter = 0;
        dataRead = false;
        startTransmission = true;
        secondResponse = false;
    }
    return retValue;
}
*/
}
//Dos modos de uso, si se busca una respuesta específica, se pasa la respuesta a esperar en expectedResponse, y se pasa NULL en responseToThis y resultResponseToThis
//Si se busca una respuesta que contenga algo a confirmar, se pasa NULL en expectedResponse, y se pasa el string que precede a lo buscado en responseToThis, y se pasa un puntero a un string vacío en resultResponseToThis
uint8_t SIM800L_CheckResponse(uint8_t *response, uint8_t *expectedResponse, uint8_t *responseToThis, uint8_t *resultResponseToThis) {
    uint8_t *token;
    
    if(expectedResponse != NULL) {
        if(strstr(response, expectedResponse) != NULL) {
            return FOUND;
        } else {
            return NOT_FOUND;
        }
    } else {
        token = strtok(response, responseToThis);
        
        if(token != NULL) {
            strcpy(resultResponseToThis, token);

            return FOUND;
        } else {
            return NOT_FOUND;
        }
    }
}
