#ifndef STM32_DRIVER_
#define STM32_DRIVER_

#define VALUE_ERROR -1
#define VALUE_OK 0

//Decl func
void SystemInit(void);
int bitWrite(int*, int, int);
void blockWrite(int*, int, int);
void UARTConf(int*);

#endif
