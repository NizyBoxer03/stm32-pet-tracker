#include "sim800l.h"

extern UART_HandleTypeDef huart3;

extern uint8_t SIM800L_response[BUF_SIZE];

extern bool simResponseReady;

//Command packs
const char *SMS_PACK[] = {
    "AT+CFUN=1\n\r",
    "AT+CMGF=1\n\r",
    "AT+CSCA=\"+541151740055\"\n\r",
    "AT+CMGS=\"+5491134285648\"\n\r",
    "SMS\n\r" //\0x1A
};

const char *HTTP_PACK[] = {
    "AT+CFUN=1\n\r", //OK
    "AT+CSTT=\"datos.personal.com\"\n\r", //OK
	"AT+CIICR\n\r", //OK
	"AT+CIFSR\n\r", //DEVUELVE LA IP
    "AT+SAPBR=3,1,\"Contype\",\"GPRS\"\n\r", //OK
    "AT+SAPBR=3,1,\"APN\",\"datos.personal.com\"\n\r", //OK
    "AT+SAPBR=1,1\n\r", //OK
    "AT+HTTPINIT\n\r", //OK
	"AT+HTTPSSL=0\n\r", //OK
    "AT+HTTPPARA=\"CID\",1\n\r", //OK
    "WEBSERVER\n\r", //OK
	"AT+HTTPACTION=0\n\r", //OK Y LUEGO 200 O ALGUN ERROR
    "AT+HTTPTERM\n\r", //OK
};

const char *RESET_PACK[] = {
	"AT+HTTPTERM\n\r",
	"AT+CIPSHUT\n\r",
    "AT+SAPBR=0,1\n\r"
};

const char BATTERY[] = "AT+CBC\n\r";

const char *POWER_PACK[] = {
		"AT+CSCLK=2\n\r",
		"AT\n\r",
		"AT+CSCLK=0\n\r"
};

//HAL_PWR_EnterSLEEPMode();

//Function implementations
void SIM800L_Reset()
{
    for(int i = 0; i < 3; i++)
    {
    	HAL_UART_Transmit_DMA(&huart3, RESET_PACK[i], strlen((char*)RESET_PACK[i]));
    	HAL_Delay(1000);
    }
}

uint8_t SIM800L_BatteryCheck(uint16_t *voltage)
{
    static uint8_t retryCounter = 0;
    static bool startTransmission = true;
    static bool secondResponse = false;
    static bool dataRead = false;

    static uint8_t batteryVoltage[10];
    uint8_t idx = 0;

    uint8_t retValue;

    if(startTransmission) {
    	HAL_Delay(1000);
    	simResponseReady = false;
        HAL_UART_Transmit_DMA(&huart3, BATTERY, strlen((char*)BATTERY));
        startTransmission = false;
        //HAL_Delay(1000);
        //simResponseReady = false;
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

        //HAL_Delay(250);
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
        HAL_UART_Transmit_DMA(&huart3, SMS_PACK[i], strlen((char*)SMS_PACK[i]));
        if(i == 3) {
            HAL_Delay(200);
            HAL_UART_Transmit_DMA(&huart3, sms, strlen((char*) sms));
        }
        startTransmission = false;
    }
    if(simResponseReady) {
       switch(i)
        {                
            case 3:
                if(!secondResponse) 
                {
                    if(SIM800L_response[0] != NULL) 
                    {
                        secondResponse = true;
                        retValue = CONTINUE;
                    } 
                    else 
                    {
                        retValue = SMS_RETRY; 
                    }
                } 
                else 
                {
                    if(SIM800L_CheckResponse(SIM800L_response, "OK", NULL, NULL) == FOUND) 
                    {
                        secondResponse = false;
                        retValue = SUCCESS;
                    } 
                    else 
                    {
                        retValue = SMS_RETRY; 
                    }
                }
                break;
            
            default:
                if(SIM800L_CheckResponse(SIM800L_response, "OK", NULL, NULL) == FOUND) 
                {
                    retValue = CONTINUE;
                } 
                else 
                {
                    retValue = RETRY; 
                }
                
                break;
        }

        dataRead = true;
        simResponseReady = false;
    } else {
        dataRead = false;
        retValue = NO_READ;
    }

    if(retValue == RETRY || retValue == SMS_RETRY) {
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

uint8_t SIM800L_WEBUpdate(uint8_t *url)  //It finally works, after countless hours of debugging
{
    static uint8_t i = 0;
    static uint8_t retryCounter = 0;
    static bool startTransmission = true;
    static bool secondResponse = false;
    static bool dataRead = false;

    uint8_t retValue;
 
    if(startTransmission) {
        if(i == 10) {
            HAL_UART_Transmit_DMA(&huart3, url, strlen((char*) url));
            //HAL_Delay(500);
        } else {
            HAL_UART_Transmit_DMA(&huart3, HTTP_PACK[i], strlen((char*)HTTP_PACK[i]));
            //HAL_Delay(500);
        }
        startTransmission = false;
    }
    if(simResponseReady) {
        switch(i)
        {
        	case 3: //CIFSR
                if(SIM800L_response[0] != NULL) 
                {
                    retValue = CONTINUE;
                } 
                else 
                {
                    retValue = RETRY;
                }

                break;

            case 11: //HTTPACTION
                if(!secondResponse) 
                {
                    if(SIM800L_CheckResponse(SIM800L_response, "OK", NULL, NULL) == FOUND) 
                    {
                        secondResponse = true;
                        retValue = CONTINUE;
                    } 
                    else 
                    {
                        retValue = WEB_RETRY;
                    }

                } 
                else 
                {
                    if(SIM800L_CheckResponse(SIM800L_response, "200", NULL, NULL) == FOUND) {
                        secondResponse = false;
                        retValue = CONTINUE;
                    } 
                    else 
                    {
                        retValue = WEB_RETRY;
                    }
                }
                    break;

            default:
                if(SIM800L_CheckResponse(SIM800L_response, "OK", NULL, NULL) == FOUND)
                {
                    if(i == 12) retValue = SUCCESS;
                    else retValue = CONTINUE;
                } 
                else 
                {
                    retValue = RETRY;
                }

                break;
        }

        dataRead = true;
        simResponseReady = false;

    } else {
        dataRead = false; //herein lies part of the problem: always no read
        retValue = NO_READ;
    }

    if(retValue == RETRY || retValue == WEB_RETRY) 
    {
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
    if(retValue == SUCCESS || retValue == FATAL_ERROR) 
    {
        i = 0;
        //if(retValue == SUCCESS) i = 0;
        if(retValue == FATAL_ERROR) SIM800L_Reset();
        retryCounter = 0;
        dataRead = false;
        startTransmission = true;
        secondResponse = false;
    }
    return retValue;
}

void SIM800L_Sleep()
{
	SIM800L_Reset();

	HAL_UART_Transmit_DMA(&huart3, POWER_PACK[0], strlen((char*)POWER_PACK[0]));
	HAL_Delay(5000);

}

void SIM800L_Wakeup() //In theory, the auto sleep function should work but I'm not taking any chances, m'kay
{
	for(int i = 1; i < 3; i++)
	{
		HAL_UART_Transmit_DMA(&huart3, POWER_PACK[i], strlen((char*)POWER_PACK[i]));
		HAL_Delay(500);
	}
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
