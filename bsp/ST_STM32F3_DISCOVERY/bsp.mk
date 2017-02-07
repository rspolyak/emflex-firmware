include $(CHIBIOS)/os/hal/boards/$(TRG)/board.mk
include $(CHIBIOS)/os/common/ports/ARMCMx/compilers/GCC/mk/startup_stm32f3xx.mk
include $(CHIBIOS)/os/hal/ports/STM32/STM32F3xx/platform.mk
LDSCRIPT= $(STARTUPLD)/STM32F303xC.ld
MCU=cortex-m4

#CSRC+=$(patsubst %.c, %.o, $(wildcard drivers/sensors/gps_api.c))
CSRC+=$(patsubst %.c, %.o, $(wildcard drivers/*/*.c))
CSRC+=$(patsubst %.c, %.o, $(wildcard drivers/*.c))
