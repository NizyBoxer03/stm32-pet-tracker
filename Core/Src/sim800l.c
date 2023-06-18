#include "sim800l.h"

extern UART_HandleTypeDef huart2;

enum {
    ATCheck = 0,
    ATSIMCheck = 1,
    ATFullFunctionality = 2,
    ATSMSMode = 3,
    ATSignalQuality = 4,
    ATCOPS = 5,
    ATCREG = 6,
    ATPhoneCarrier = 7,
    ATSMS2Phone = 8,
    ATSMSMessage = 9
};

const char *SIM800L_Commands[] = {
    "AT\n\r",
    "AT+CPIN?\n\r",
    "AT+CFUN=1\n\r",
    "AT+CMGF=1\n\r",
    "AT+CSQ\n\r",
    "AT+COPS?\n\r",
    "AT+CREG?\n\r",
    "AT+CSCA=\"+543200000001\"\n\r",
    "AT+CMGS=\"+5491123922468\"\n\r",
    "https://www.google.com/maps/place/-34.6171807,-58.3823728\x1A"
};

void SIM800L_Init() {
    for (uint8_t i = 0; i < 4; i++) {
        HAL_UART_Transmit_DMA(&huart2, SIM800L_Commands[i], strlen((char*)SIM800L_Commands[i]));
        HAL_Delay(1000);
    }
}

void SIM800L_CheckStatus() {
    for (uint8_t i = 4; i < 7; i++) {
        HAL_UART_Transmit_DMA(&huart2, SIM800L_Commands[i], strlen((char*)SIM800L_Commands[i]));
        HAL_Delay(1000);
    }
}

void SIM800L_SMSConfig() {
    for (uint8_t i = 7; i < 8; i++) {
        HAL_UART_Transmit_DMA(&huart2, SIM800L_Commands[i], strlen((char*)SIM800L_Commands[i]));
        HAL_Delay(1000);
    }
}

void SIM800L_SendMessage(){
    for (uint8_t i = 8; i < 10; i++) {
        HAL_UART_Transmit_DMA(&huart2, SIM800L_Commands[i], strlen((char*)SIM800L_Commands[i]));
        HAL_Delay(1000);
    }
}
