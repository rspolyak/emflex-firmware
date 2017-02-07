include $(CHIBIOS)/os/hal/boards/$(TRG)/board.mk
include $(CHIBIOS)/os/common/ports/ARMCMx/compilers/GCC/mk/startup_stm32l1xx.mk
include $(CHIBIOS)/os/hal/ports/STM32/STM32L1xx/platform.mk
LDSCRIPT= $(STARTUPLD)/STM32L152xB.ld
MCU  = cortex-m3

CSRC+=$(patsubst %.c, %.o, $(wildcard drivers/*/*.c))
CSRC+=$(patsubst %.c, %.o, $(wildcard drivers/*.c))
