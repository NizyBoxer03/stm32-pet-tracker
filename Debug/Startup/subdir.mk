################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Startup/startup_stm32f103c8.c 

OBJS += \
./Startup/startup_stm32f103c8.o 

C_DEPS += \
./Startup/startup_stm32f103c8.d 


# Each subdirectory must supply rules for building sources it contributes
Startup/startup_stm32f103c8.o: ../Startup/startup_stm32f103c8.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DSTM32 -DSTM32F1 -DSTM32F103C8Tx -DDEBUG -c -I../Inc -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Startup/startup_stm32f103c8.d" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

