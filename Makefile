######################################
# path
######################################
_PATH = $(subst /,\,$(abspath .))

######################################
# target
######################################
TARGET = STM32_Project

######################################
# building variables
######################################
# debug build?
DEBUG = 1

#######################################
# paths
#######################################
# Build path
BUILD_DIR = Build

######################################
# source
######################################
# C sources
C_SOURCES =  \
Src/main.c \
Src/stm32f103c8_driver.c \
Startup/startup_stm32f103c8.c

#######################################
# binaries
#######################################
PREFIX = arm-none-eabi-
# The gcc compiler bin path can be either defined in make command via GCC_PATH variable (> make GCC_PATH=xxx)
# either it can be added to the PATH environment variable.

CC = $(PREFIX)gcc
CP = $(PREFIX)objcopy
SZ = $(PREFIX)size

HEX = $(CP) -O ihex
BIN = $(CP) -O binary
 
#######################################
# CFLAGS
#######################################
# cpu
CPU = -mcpu=cortex-m3

# mcu
MCU = $(CPU) -mthumb 

# C includes
C_INCLUDES =  \
-I Inc \
-I Drivers/Inc

# compile gcc flags
CFLAGS = $(MCU) -nostdlib

ifeq ($(DEBUG), 1)
CFLAGS += -g -gdwarf-2
endif

#######################################
# LDFLAGS
#######################################
# link script
LDSCRIPT = STM32F103C8TX_FLASH.ld

LDFLAGS = -Wl,-T $(LDSCRIPT)

# default action: build all
all: $(BUILD_DIR)/$(TARGET).elf $(BUILD_DIR)/$(TARGET).hex $(BUILD_DIR)/$(TARGET).bin
ifeq ($(DEBUG), 1)
	@cmd.exe /c "mklink $(_PATH)\$(BUILD_DIR)\main.c $(_PATH)\Src\main.c"
	@cmd.exe /c "mklink $(_PATH)\$(BUILD_DIR)\stm32f103c8_driver.c $(_PATH)\Src\stm32f103c8_driver.c"
	@cmd.exe /c "mklink $(_PATH)\$(BUILD_DIR)\startup_stm32f103c8.c $(_PATH)\startup_stm32f103c8.c"
endif

#######################################
# build the application
#######################################
# list of objects
OBJECTS = $(addprefix $(BUILD_DIR)/,$(notdir $(C_SOURCES:.c=.o)))
vpath %.c $(sort $(dir $(C_SOURCES)))

$(BUILD_DIR)/%.o: %.c Makefile | $(BUILD_DIR) 
	$(CC) -c $(CFLAGS) $(C_INCLUDES) $< -o $@

$(BUILD_DIR)/$(TARGET).elf: $(OBJECTS) Makefile
	$(CC) $(OBJECTS) $(LDFLAGS) $(CFLAGS) -o $@
	$(SZ) $@

$(BUILD_DIR)/%.hex: $(BUILD_DIR)/%.elf | $(BUILD_DIR)
	$(HEX) $< $@
	
$(BUILD_DIR)/%.bin: $(BUILD_DIR)/%.elf | $(BUILD_DIR)
	$(BIN) $< $@	
	
$(BUILD_DIR):
	mkdir $@		

#######################################
# clean up
#######################################
clean: $(BUILD_DIR)
	-rm -fR $(BUILD_DIR)

# *** EOF ***