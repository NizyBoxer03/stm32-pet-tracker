#include "STM32_driver.h"
#include "STM32_reg.h"

void SystemInit(void) 
{
	bitWrite(RCC_APB2ENR_ptr, 4, 1);
	/*
	bitWrite(GPIOC_CRH_ptr, 20, 1);
	bitWrite(GPIOC_CRH_ptr, 21, 1);
	bitWrite(GPIOC_CRH_ptr, 22, 0);
	bitWrite(GPIOC_CRH_ptr, 23, 0);
	*/
	blockWrite(GPIOC_CRH_ptr, 20, 0b1100);
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

void blockWrite(int* ADDR, int shift, int block) //Sin testear xd
{
	*ADDR |= (block << shift); //escribir 1s
	*ADDR &= (block << shift); //escribir 0s
}

void UARTConf(int* UART_BASE_ADDR) //TEST!! Config para el GPS. Parametrizar luego. Probablemente no funciona kek
{
	//UART CONFIG
	//Clock EN (UART 1!!)
	bitWrite(RCC_APB2ENR_ptr, 14, 1);

	//BRR (this is an absolute bitch to program)
	//According to the formula, USARTDIV = fpclk/(16*baud). At 8 MHz & 9600 baud, the value should be 52.0833333
	//DIV_FRAC = 16*0d0.83333... = 13.333... -> 13 -> 0xD
	//DIV_MANT = 52 + (no carry) = 52 (kek) -> 0x34
	blockWrite(((int*)(UART_BASE_ADDR+0x08)), 4, 0x34); //MANT
	blockWrite(((int*)(UART_BASE_ADDR+0x0C)), 0, 0xD); //FRAC

	//CR1
	bitWrite(((int*)(UART_BASE_ADDR+0x0C)), 13, 1); //USART EN
	bitWrite(((int*)(UART_BASE_ADDR+0x0C)), 12, 0); //W LEN: 1s, 8d, ns
	bitWrite(((int*)(UART_BASE_ADDR+0x0C)), 11, 0); //WAKE: IDLE
	bitWrite(((int*)(UART_BASE_ADDR+0x0C)), 10, 0); //PAR DISABLE
	//bitWrite(((int*)(UART_BASE_ADDR+0x0C)), 9, 0); //PAR SEL
	//bitWrite(((int*)(UART_BASE_ADDR+0x0C)), 8, 0); //PEIE EN
	//bitWrite(((int*)(UART_BASE_ADDR+0x0C)), 7, 1); //TXEIE EN
	//bitWrite(((int*)(UART_BASE_ADDR+0x0C)), 6, 1); //TCIE EN
	//bitWrite(((int*)(UART_BASE_ADDR+0x0C)), 5, 1); //RXNE EN
	//bitWrite(((int*)(UART_BASE_ADDR+0x0C)), 4, 1); //IDLE EN
	//bitWrite(((int*)(UART_BASE_ADDR+0x0C)), 3, 1); //TX EN
	//bitWrite(((int*)(UART_BASE_ADDR+0x0C)), 2, 1); //RX EN
	//bitWrite(((int*)(UART_BASE_ADDR+0x0C)), 1, 1); //Receiver Wakeup
	//bitWrite(((int*)(UART_BASE_ADDR+0x0C)), 0, 1); //Send Break

	//CR2
	bitWrite(((int*)(UART_BASE_ADDR+0x10)), 12, 0); //STOP BIT 0
	bitWrite(((int*)(UART_BASE_ADDR+0x10)), 13, 0); //STOP BIT 1, 00 = 1 STOP BIT

	//CR3
	//bitWrite((int*)(UART_BASE_ADDR+0x14)), 7, 0); //DMA TX
	//bitWrite(((int*)(UART_BASE_ADDR+0x14)), 6, 0); //DMA RX
	//bitWrite(((int*)(UART_BASE_ADDR+0x14)), 0, 0); //DMA EIE EN

}
