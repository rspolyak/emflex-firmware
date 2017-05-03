include bsp/$(TRG)/board.mk
include $(CHIBIOS)/os/common/ports/ARMCMx/compilers/GCC/mk/startup_stm32f1xx.mk
include $(CHIBIOS)/os/hal/ports/STM32/STM32F1xx/platform.mk
LDSCRIPT= bsp/$(TRG)/STM32F103x8.ld

MCU  = cortex-m3

#SRC:=

CSRC+=$(patsubst %.c, %.o, $(SRC))
