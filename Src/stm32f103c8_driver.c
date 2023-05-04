#include "STM32_driver.h"
#include "STM32_reg.h"

void SystemInit(void) 
{
	RCC_APB2ENR |= (1<<4); //GPIOC
	//bitWrite(RCC_APB2ENR_ptr, 4, 1);
	//GPIOC_CRH &= ~(0xFF<<20); //CNF13 = 00, MODE13 = 00
	bitWrite(GPIOC_CRH_ptr, 20, 1);
	bitWrite(GPIOC_CRH_ptr, 21, 1);
	bitWrite(GPIOC_CRH_ptr, 22, 0);
	bitWrite(GPIOC_CRH_ptr, 23, 0);

}

int bitWrite(int* ADDR, int n_bits, int state)
{
	int rValue = VALUE_OK;

	switch(state)
	{
		case 0:
			*ADDR &= ~(1<<n_bits);
			break;

		case 1:
			*ADDR |= (1<<n_bits);
			break;

		default:
			rValue = VALUE_ERROR;
			break;
	}

	return rValue;
}

void blockWrite(int* ADDR, int shift, int block)
{
	*ADDR |= (block << shift); //escribir 1s
	*ADDR &= (block << shift); //escribir 0s
}

void UARTConf(int* UART_BASE_ADDR) //TEST!! Config para el GPS. Parametrizar luego
{
	//UART CONFIG
	bitWrite((UART_BASE_ADDR+0x0C), 13, 1); //USART EN
	bitWrite((UART_BASE_ADDR+0x0C), 12, 0); //W LEN: 1s, 8d, ns
	bitWrite((UART_BASE_ADDR+0x0C), 11, 0); //WAKE: IDLE
	bitWrite((UART_BASE_ADDR+0x0C), 10, 0); //PAR DISABLE
	//bitWrite((UART_BASE_ADDR+0x0C), 7, 1); //TXEIE EN
	//bitWrite((UART_BASE_ADDR+0x0C), 6, 1); //TCIE EN
}
