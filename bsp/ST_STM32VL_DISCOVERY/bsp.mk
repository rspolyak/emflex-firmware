include $(CHIBIOS)/os/hal/boards/$(TRG)/board.mk
include $(CHIBIOS)/os/common/ports/ARMCMx/compilers/GCC/mk/startup_stm32f1xx.mk
include $(CHIBIOS)/os/hal/ports/STM32/STM32F1xx/platform.mk
LDSCRIPT= $(STARTUPLD)/STM32F100xB.ld

CSRC+=$(patsubst %.c, %.o, $(wildcard drivers/sensors/gps_api.c))

CSRC+=$(patsubst %.c, %.o, $(wildcard drivers/network/wifi.c))
