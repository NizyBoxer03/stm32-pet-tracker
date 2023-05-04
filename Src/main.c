//Basado en el archivo main_modelo_blink.c de Teams
#include "STM32_reg.h"
#include "STM32_driver.h"

int i = 0, a = 16, c = 340, effefd = 33245;
int b;

int main()
{
	//GPIOC CLK Enable
	//RCC_APB2ENR |= (1<<4);
	bitWrite(RCC_APB2ENR_ptr, 4, 1);

	/*
	GPIOC_CRH |= (1<<20); //OR para escribir 1s
	GPIOC_CRH |= (1<<21);
	GPIOC_CRH &= ~(1<<22); //AND para escribir 0s
	GPIOC_CRH &= ~(1<<23);
	

	bitWrite(GPIOC_CRH_ptr, 20, 1);
	bitWrite(GPIOC_CRH_ptr, 21, 1);
	bitWrite(GPIOC_CRH_ptr, 22, 0);
	bitWrite(GPIOC_CRH_ptr, 23, 0);
	*/

	//Loop
	while(1)
	{
		//GPIOC_ODR |= (1<<13); //apag
		bitWrite(GPIOC_ODR_ptr, 13, 1);

		for(volatile int i = 0; i < 50000; i++); //delay
			
		//GPIOC_ODR &= ~(1<<13); //enc
		bitWrite(GPIOC_ODR_ptr, 13, 0);

		for(volatile int i = 0; i < 50000; i++); //delay
	}
	
	return 0;
}

