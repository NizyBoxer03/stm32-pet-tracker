#ifndef STM32_REG_
#define STM32_REG_

#define RCC_APB2ENR (*((int*)(0x40021000U + 0x18U))) //hecho por el profe
#define GPIOC_BASE 0x40011000U
#define GPIOC_CRH (*((int*)(GPIOC_BASE + 0x04U)))
#define GPIOC_ODR (*((int*)(GPIOC_BASE + 0x0CU)))
#define GPIOC_BSRR (*((int*)(GPIOC_BASE + 0x10U)))

#define RCC_APB2ENR_ptr ((int*)(0x40021000U + 0x18U)) //hecho por el profe
#define GPIOC_CRH_ptr ((int*)(GPIOC_BASE + 0x04U))
#define GPIOC_ODR_ptr ((int*)(GPIOC_BASE + 0x0CU))
#define GPIOC_BSRR_ptr ((int*)(GPIOC_BASE + 0x10U))

#define UART1_BASE 0x40013800U //UART1: GPS
#define UART2_BASE 0x40004400U //UART2: SIM

#endif
