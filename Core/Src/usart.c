#include "usart.h"
#include "nmea.h"
#include "sim800l.h"

extern UART_HandleTypeDef huart2;
extern UART_HandleTypeDef huart3;

uint8_t usart2_buffer[2] = {0};
uint8_t usart3_buffer[2] = {0};

uint8_t sentence[NMEA0183_MAX_LENGTH + 1] = {0};
uint8_t SIM800L_response[BUF_SIZE] = {0};

bool gpsSentenceReady = false;
bool simResponseReady = false;

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{	
    /* start the DMA again */
    if (huart->Instance == USART2)
	{   
        if(GPS_RX("GPRMC") == SUCCESS) gpsSentenceReady = true;
        else HAL_UART_Receive_DMA(&huart2, usart2_buffer, 1);
	}

	/* start the DMA again */
	if (huart->Instance == USART3)
	{
        if(SIM_RX() == SUCCESS) simResponseReady = true;
        
        HAL_UART_Receive_DMA(&huart3, usart3_buffer, 1);
	}
}

uint8_t GPS_RX(uint8_t *header) {
    static uint8_t idx, cont;
    static uint8_t gps_buffer[NMEA0183_MAX_LENGTH + 1];
		
    if(usart2_buffer[0] == '$' || idx > 0) {
        if(usart2_buffer[0] == 0x0D) {
            gps_buffer[idx] = '\0';

            for (uint8_t i = 0; i <= idx; i++) {
                sentence[i] = gps_buffer[i];
                gps_buffer[i] = 0;
            }

            idx = 0;
            cont = 0;

            return SUCCESS;
        } else {
            gps_buffer[idx] = usart2_buffer[0];
            idx++;

            if((gps_buffer[0] == '$' && (idx - 1) == 0) || (gps_buffer[1] == header[0] && (idx - 1) == 1) || (gps_buffer[2] == header[1] && (idx - 1) == 2) || (gps_buffer[3] == header[2] && (idx - 1) == 3) || (gps_buffer[4] == header[3] && (idx - 1) == 4) || (gps_buffer[5] == header[4] && (idx - 1) == 5)) {
                cont++;
            }
            if(idx == 6 && cont < 6) {
                idx = 0;
                cont = 0;
            }
        }
    }

    return ERROR; //Realmente no deberia ser un error pero por si las dudas, el define queda asi.
}

uint8_t SIM_RX() {
    static uint8_t idx;
    static uint8_t SIM800L_buffer[BUF_SIZE];
    static bool RialShitMoment = false;

    if((SIM800L_buffer[0] == 'A' && SIM800L_buffer[1] == 'T') || (SIM800L_buffer[0] == 'A' && SIM800L_buffer[1] == 'A' && SIM800L_buffer[2] == 'T')) {
        RialShitMoment = true;
    } 
    if(!(idx == 0 && (usart3_buffer[0] == 0x0A || usart3_buffer[0] == 0x0D))) {
        if(usart3_buffer[0] == 0x0D) {
            SIM800L_buffer[idx] = '\0';

            for(uint8_t i = 0; i <= idx; i++) {
                if(!RialShitMoment) SIM800L_response[i] = SIM800L_buffer[i];
                SIM800L_buffer[i] = 0;
            }

            idx = 0;

            if(RialShitMoment) {
                RialShitMoment = false;
                return ERROR;
            } else {
                return SUCCESS;
            }
        } else {
            SIM800L_buffer[idx] = usart3_buffer[0];
            idx++;

            return ERROR;
        }
    }
}
