#include <stdint.h>
#include "STM32_driver.h"

//importo main
extern int main(void);

//importo variables utiles del linker
extern uint32_t _estack;
extern uint32_t _sdata;
extern uint32_t _edata;
extern uint32_t _sidata;
extern uint32_t _sbss;
extern uint32_t _ebss;

void Reset_Handler(void);

//implemento reset handler
void Reset_Handler(void)
{
	uint32_t *datalnit = &_sidata;
    uint32_t *data = &_sdata;
	while(data < &_edata) {
		*data++ = *datalnit++;
	}
	
	uint32_t *bss = &_sbss;
	while(bss < &_ebss) {
		*bss++ = 0;
	}
	
	SystemInit();

	//finalmente, llamo a main
	main();
	
	while(1);
}

void Default_Handler(void) {
    while(1);
}

__attribute__((weak)) void NMI_Handler (void) { Default_Handler(); }
__attribute__((weak)) void HardFault_Handler (void) { Default_Handler(); }
__attribute__((weak)) void MemManage_Handler (void) { Default_Handler(); }
__attribute__((weak)) void BusFault_Handler (void) { Default_Handler(); }
__attribute__((weak)) void UsageFault_Handler (void) { Default_Handler(); }
__attribute__((weak)) void SVC_Handler (void) { Default_Handler(); }
__attribute__((weak)) void DebugMon_Handler (void) { Default_Handler(); }
__attribute__((weak)) void PendSV_Handler (void) { Default_Handler(); }
__attribute__((weak)) void SysTick_Handler (void) { Default_Handler(); }
__attribute__((weak)) void WWDG_IRQHandler (void) { Default_Handler(); }          			/* Window Watchdog interrupt                        */
__attribute__((weak)) void PVD_IRQHandler (void) { Default_Handler(); }          			/* PVD through EXTI line detection interrupt        */
__attribute__((weak)) void TAMPER_IRQHandler (void) { Default_Handler(); }       			/* Tamper interrupt                                 */
__attribute__((weak)) void RTC_IRQHandler (void) { Default_Handler(); }          			/* RTC global interrupt                             */
__attribute__((weak)) void FLASH_IRQHandler (void) { Default_Handler(); }        			/* Flash global interrupt                           */
__attribute__((weak)) void RCC_IRQHandler (void) { Default_Handler(); }          			/* RCC global interrupt                             */
__attribute__((weak)) void EXTI0_IRQHandler (void) { Default_Handler(); }        			/* EXTI Line0 interrupt                             */
__attribute__((weak)) void EXTI1_IRQHandler (void) { Default_Handler(); }        			/* EXTI Line1 interrupt                             */
__attribute__((weak)) void EXTI2_IRQHandler (void) { Default_Handler(); }        			/* EXTI Line2 interrupt                             */
__attribute__((weak)) void EXTI3_IRQHandler (void) { Default_Handler(); }        			/* EXTI Line3 interrupt                             */
__attribute__((weak)) void EXTI4_IRQHandler (void) { Default_Handler(); }        			/* EXTI Line4 interrupt                             */
__attribute__((weak)) void DMA1_Channel1_IRQHandler (void) { Default_Handler(); } 			/* DMA1 Channel1 global interrupt                   */
__attribute__((weak)) void DMA1_Channel2_IRQHandler (void) { Default_Handler(); } 			/* DMA1 Channel2 global interrupt                   */
__attribute__((weak)) void DMA1_Channel3_IRQHandler (void) { Default_Handler(); }			/* DMA1 Channel3 global interrupt                   */
__attribute__((weak)) void DMA1_Channel4_IRQHandler (void) { Default_Handler(); }			/* DMA1 Channel4 global interrupt                   */
__attribute__((weak)) void DMA1_Channel5_IRQHandler (void) { Default_Handler(); }			/* DMA1 Channel5 global interrupt                   */
__attribute__((weak)) void DMA1_Channel6_IRQHandler (void) { Default_Handler(); }			/* DMA1 Channel6 global interrupt                   */
__attribute__((weak)) void DMA1_Channel7_IRQHandler (void) { Default_Handler(); }			/* DMA1 Channel7 global interrupt                   */
__attribute__((weak)) void ADC1_2_IRQHandler (void) { Default_Handler(); }       			/* ADC1 and ADC2 global interrupt                   */
__attribute__((weak)) void USB_HP_CAN_TX_IRQHandler (void) { Default_Handler(); }			/* USB High Priority or CAN TX interrupts           */
__attribute__((weak)) void USB_LP_CAN_RX0_IRQHandler (void) { Default_Handler(); }			/* USB Low Priority or CAN RX0 interrupts           */
__attribute__((weak)) void CAN_RX1_IRQHandler (void) { Default_Handler(); }      			/* CAN RX1 interrupt                                */
__attribute__((weak)) void CAN_SCE_IRQHandler (void) { Default_Handler(); }      			/* CAN SCE interrupt                                */
__attribute__((weak)) void EXTI9_5_IRQHandler (void) { Default_Handler(); }      			/* EXTI Line[9:5] interrupts                        */
__attribute__((weak)) void TIM1_BRK_IRQHandler (void) { Default_Handler(); }     			/* TIM1 Break interrupt                             */
__attribute__((weak)) void TIM1_UP_IRQHandler (void) { Default_Handler(); }      			/* TIM1 Update interrupt                            */
__attribute__((weak)) void TIM1_TRG_COM_IRQHandler (void) { Default_Handler(); }  			/* TIM1 Trigger and Commutation interrupts          */
__attribute__((weak)) void TIM1_CC_IRQHandler (void) { Default_Handler(); }       			/* TIM1 Capture Compare interrupt                   */
__attribute__((weak)) void TIM2_IRQHandler (void) { Default_Handler(); }          			/* TIM2 global interrupt                            */
__attribute__((weak)) void TIM3_IRQHandler (void) { Default_Handler(); }         			/* TIM3 global interrupt                            */
__attribute__((weak)) void TIM4_IRQHandler (void) { Default_Handler(); }        	    	/* TIM4 global interrupt                            */
__attribute__((weak)) void I2C1_EV_IRQHandler (void) { Default_Handler(); }      			/* I2C1 event interrupt                             */
__attribute__((weak)) void I2C1_ER_IRQHandler (void) { Default_Handler(); }      			/* I2C1 error interrupt                             */
__attribute__((weak)) void I2C2_EV_IRQHandler (void) { Default_Handler(); }      			/* I2C2 event interrupt                             */
__attribute__((weak)) void I2C2_ER_IRQHandler (void) { Default_Handler(); }      			/* I2C2 error interrupt                             */
__attribute__((weak)) void SPI1_IRQHandler (void) { Default_Handler(); }         			/* SPI1 global interrupt                            */
__attribute__((weak)) void SPI2_IRQHandler (void) { Default_Handler(); }         			/* SPI2 global interrupt                            */
__attribute__((weak)) void USART1_IRQHandler (void) { Default_Handler(); }        			/* USART1 global interrupt                          */
__attribute__((weak)) void USART2_IRQHandler (void) { Default_Handler(); }       			/* USART2 global interrupt                          */
__attribute__((weak)) void USART3_IRQHandler (void) { Default_Handler(); }       			/* USART3 global interrupt                          */
__attribute__((weak)) void EXTI15_10_IRQHandler (void) { Default_Handler(); }    			/* EXTI Line[15:10] interrupts                      */
__attribute__((weak)) void RTCAlarm_IRQHandler (void) { Default_Handler(); }     			/* RTC Alarms through EXTI line interrupt           */
__attribute__((weak)) void TIM8_BRK_IRQHandler (void) { Default_Handler(); }     			/* TIM8 Break interrupt                             */
__attribute__((weak)) void TIM8_UP_IRQHandler (void) { Default_Handler(); }      			/* TIM8 Update interrupt                            */
__attribute__((weak)) void TIM8_TRG_COM_IRQHandler (void) { Default_Handler(); } 			/* TIM8 Trigger and Commutation interrupts          */
__attribute__((weak)) void TIM8_CC_IRQHandler (void) { Default_Handler(); }      			/* TIM8 Capture Compare interrupt                   */
__attribute__((weak)) void ADC3_IRQHandler (void) { Default_Handler(); }         			/* ADC3 global interrupt                            */
__attribute__((weak)) void FSMC_IRQHandler (void) { Default_Handler(); }         			/* FSMC global interrupt                            */
__attribute__((weak)) void SDIO_IRQHandler (void) { Default_Handler(); }         			/* SDIO global interrupt                            */
__attribute__((weak)) void TIM5_IRQHandler (void) { Default_Handler(); }         			/* TIM5 global interrupt                            */
__attribute__((weak)) void SPI3_IRQHandler (void) { Default_Handler(); }         			/* SPI3 global interrupt                            */
__attribute__((weak)) void UART4_IRQHandler (void) { Default_Handler(); }        			/* UART4 global interrupt                           */
__attribute__((weak)) void UART5_IRQHandler (void) { Default_Handler(); }        			/* UART5 global interrupt                           */
__attribute__((weak)) void TIM6_IRQHandler (void) { Default_Handler(); }         			/* TIM6 global interrupt                            */
__attribute__((weak)) void TIM7_IRQHandler (void) { Default_Handler(); }         			/* TIM7 global interrupt                            */
__attribute__((weak)) void DMA2_Channel1_IRQHandler (void) { Default_Handler(); }			/* DMA2 Channel1 global interrupt                   */
__attribute__((weak)) void DMA2_Channel2_IRQHandler (void) { Default_Handler(); }			/* DMA2 Channel2 global interrupt                   */
__attribute__((weak)) void DMA2_Channel3_IRQHandler (void) { Default_Handler(); }			/* DMA2 Channel3 global interrupt                   */
__attribute__((weak)) void DMA2_Channel4_5_IRQHandler (void) { Default_Handler(); }			/* DMA2 Channel4 and DMA2 Channel5 global interrupt */

//implemento tabla de interrupts

__attribute__((section(".isr_vector")))
const void (*VectorTable[])(void) = {
    (const void (*)(void)) &_estack,
    Reset_Handler,
	NMI_Handler,
	HardFault_Handler,
	MemManage_Handler,
	BusFault_Handler,
	UsageFault_Handler,
	0,
	0,
	0,
	0,
	SVC_Handler,
	DebugMon_Handler,
	0,
	PendSV_Handler,
	SysTick_Handler,
	WWDG_IRQHandler,           		
	PVD_IRQHandler,            		
	TAMPER_IRQHandler,         		
	RTC_IRQHandler,            		
	FLASH_IRQHandler,          		
	RCC_IRQHandler,            		
	EXTI0_IRQHandler,          		
	EXTI1_IRQHandler,          		
	EXTI2_IRQHandler,          		
	EXTI3_IRQHandler,          		
	EXTI4_IRQHandler,          		
	DMA1_Channel1_IRQHandler,  		
	DMA1_Channel2_IRQHandler,  		
	DMA1_Channel3_IRQHandler,  		
	DMA1_Channel4_IRQHandler,  		
	DMA1_Channel5_IRQHandler,  		
	DMA1_Channel6_IRQHandler,  		
	DMA1_Channel7_IRQHandler,  		
	ADC1_2_IRQHandler,         		
	USB_HP_CAN_TX_IRQHandler,  		
	USB_LP_CAN_RX0_IRQHandler, 		
	CAN_RX1_IRQHandler,        		
	CAN_SCE_IRQHandler,        		
	EXTI9_5_IRQHandler,        		
	TIM1_BRK_IRQHandler,       		
	TIM1_UP_IRQHandler,        		
	TIM1_TRG_COM_IRQHandler,   		
	TIM1_CC_IRQHandler,        		
	TIM2_IRQHandler,           		
	TIM3_IRQHandler,           		
	TIM4_IRQHandler,           		
	I2C1_EV_IRQHandler,        		
	I2C1_ER_IRQHandler,        		
	I2C2_EV_IRQHandler,        		
	I2C2_ER_IRQHandler,        		
	SPI1_IRQHandler,           		
	SPI2_IRQHandler,           		
	USART1_IRQHandler,         		
	USART2_IRQHandler,         		
	USART3_IRQHandler,         		
	EXTI15_10_IRQHandler,      		
	RTCAlarm_IRQHandler,       		
	0,                         		
	TIM8_BRK_IRQHandler,       		
	TIM8_UP_IRQHandler,        		
	TIM8_TRG_COM_IRQHandler,   		
	TIM8_CC_IRQHandler,        		
	ADC3_IRQHandler,           		
	FSMC_IRQHandler,           		
	SDIO_IRQHandler,           		
	TIM5_IRQHandler,           		
	SPI3_IRQHandler,           		
	UART4_IRQHandler,          		
	UART5_IRQHandler,          		
	TIM6_IRQHandler,           		
	TIM7_IRQHandler,           		
	DMA2_Channel1_IRQHandler,  		
	DMA2_Channel2_IRQHandler,  		
	DMA2_Channel3_IRQHandler,  		
	DMA2_Channel4_5_IRQHandler
};