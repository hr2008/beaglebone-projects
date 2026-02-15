# -------------------------------------------------
# Toolchain
# -------------------------------------------------
CROSS   = arm-none-eabi
CC      = $(CROSS)-gcc
LD      = $(CROSS)-ld
OBJCOPY = $(CROSS)-objcopy
OBJDUMP = $(CROSS)-objdump
SIZE    = $(CROSS)-size
AR	  	= $(CROSS)-ar
# -------------------------------------------------
# Target
# -------------------------------------------------
TARGET  = freeRTOS_app
BUILD   = build

# CPU and architecture flags
CPU = cortex-a8
ARCH = armv7-a

# -------------------------------------------------
# Flags
# -------------------------------------------------
# Compiler flags
CFLAGS = -mcpu=$(CPU)
CFLAGS += -mfloat-abi=soft
CFLAGS += -Wall -Wextra -Werror
CFLAGS += -O2 -g
CFLAGS += -ffreestanding -nostdlib -nostartfiles
CFLAGS += -fno-common -ffunction-sections -fdata-sections

LDFLAGS = -T linker.ld
LDFLAGS += -nostdlib
LDFLAGS += --gc-sections 
# -------------------------------------------------
# Source discovery (recursive)
# -------------------------------------------------
C_SRCS := $(shell find ../common -name '*.c')
C_SRCS += $(shell ls *.c)
C_SRCS += $(shell ls freeRTOS_tasks/*.c)
S_SRCS := $(shell ls *.S)

# -------------------------------------------------
# Auto include directories
# -------------------------------------------------
INC_DIRS := $(sort $(dir $(shell find ../common -name '*.h')))
INC_DIRS += $(sort $(shell pwd))
INC_DIRS += $(sort $(dir $(shell find . -name '*.h')))
INC_DIRS += $(shell ls ./freeRTOS_tasks/)
INC_FLAGS := $(addprefix -I,$(INC_DIRS))


# -------------------------------------------------
# freeRTOS sources
# -------------------------------------------------
FREERTOS_DIR = ./kernel
FREERTOS_PORT_DIR = ./portable

C_SRCS += $(FREERTOS_DIR)/tasks.c
C_SRCS += $(FREERTOS_DIR)/queue.c
C_SRCS += $(FREERTOS_DIR)/list.c
C_SRCS += $(FREERTOS_DIR)/timers.c
C_SRCS += $(FREERTOS_DIR)/stream_buffer.c
C_SRCS += $(FREERTOS_DIR)/portable/MemMang/heap_4.c

CFLAGS += -I$(FREERTOS_DIR)/include
CFLAGS += -I$(FREERTOS_PORT_DIR)
CFLAGS += -DFREE_RTOS
CFLAGS += -DUART_INTERUPT

PORT_LIB =	$(FREERTOS_PORT_DIR)/libport.a


# -------------------------------------------------
# Object files (mirrored into build/)
# -------------------------------------------------
OBJS := $(C_SRCS:%.c=$(BUILD)/%.o) \
		$(S_SRCS:%.S=$(BUILD)/%.o)

# -------------------------------------------------
# Default target
# -------------------------------------------------
all: $(BUILD)/$(TARGET).bin $(BUILD)/$(TARGET).lst size

# -------------------------------------------------
# Ensure build directories exist
# -------------------------------------------------
$(BUILD):
	mkdir -p $(BUILD)

# -------------------------------------------------
# Compile rules (C)
# -------------------------------------------------
$(BUILD)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INC_FLAGS) -c $< -o $@

# -------------------------------------------------
# Compile rules (ASM)
# -------------------------------------------------
$(BUILD)/%.o: %.S
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INC_FLAGS) -c $< -o $@

# -------------------------------------------------
# Link
# -------------------------------------------------
$(BUILD)/$(TARGET).elf: $(OBJS) $(PORT_LIB) linker.ld | $(BUILD)
	$(LD) $(LDFLAGS) $(OBJS) $(PORT_LIB) -o $@

# -------------------------------------------------
# ELF -> raw binary
# -------------------------------------------------
$(BUILD)/$(TARGET).bin: $(BUILD)/$(TARGET).elf
	$(OBJCOPY) -O binary $< $@
	cp $@ ~/.

# Create disassembly listing
$(BUILD)/$(TARGET).lst: $(BUILD)/$(TARGET).elf
	@echo "OBJDUMP $@"
	$(OBJDUMP) -D $< > $@

size: $(BUILD)/$(TARGET).elf
	@echo ""
	@echo "=== Size Information ==="
	@$(SIZE) $<
	@echo ""
	@echo "=== Memory Usage ==="
	@SIZE=$$($(SIZE) -B $< | tail -1 | awk '{print $$1+$$2}'); \
	echo "Total (text+data): $$SIZE bytes"; \
	echo "Available SRAM: 65536 bytes (64KB)"; \
	PERCENT=$$(echo "scale=1; $$SIZE * 100 / 65536" | bc); \
	echo "Usage: $$PERCENT%"

# -------------------------------------------------
# Cleanup
# -------------------------------------------------
clean:
	rm -rf common
	rm -rf $(BUILD)

.PHONY: all clean size 
