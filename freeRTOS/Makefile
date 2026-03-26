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
TARGET_DDR = freeRTOS_app_ddr
BUILD     = build
BUILD_DDR = build_ddr

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

LDFLAGS_DDR = -T linker_ddr.ld
LDFLAGS_DDR += -nostdlib
LDFLAGS_DDR += --gc-sections

# DDR-specific compiler flags — inherits all CFLAGS plus DDR_HEAP macro
CFLAGS_DDR = $(CFLAGS) -DDDR_HEAP
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

# DDR object files (mirrored into build_ddr/ — compiled with CFLAGS_DDR)
OBJS_DDR := $(C_SRCS:%.c=$(BUILD_DDR)/%.o) \
			$(S_SRCS:%.S=$(BUILD_DDR)/%.o)

# -------------------------------------------------
# Default target (SRAM)
# -------------------------------------------------
all: $(BUILD)/$(TARGET).bin $(BUILD)/$(TARGET).lst size

# DDR target
ddr_image: $(BUILD_DDR)/$(TARGET_DDR).bin $(BUILD_DDR)/$(TARGET_DDR).lst size_ddr

# -------------------------------------------------
# Ensure build directories exist
# -------------------------------------------------
$(BUILD):
	mkdir -p $(BUILD)

$(BUILD_DDR):
	mkdir -p $(BUILD_DDR)

# -------------------------------------------------
# Compile rules (C) — SRAM
# -------------------------------------------------
$(BUILD)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INC_FLAGS) -c $< -o $@

# -------------------------------------------------
# Compile rules (ASM) — SRAM
# -------------------------------------------------
$(BUILD)/%.o: %.S
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INC_FLAGS) -c $< -o $@

# -------------------------------------------------
# Compile rules (C) — DDR (uses CFLAGS_DDR = CFLAGS + -DDDR_HEAP)
# -------------------------------------------------
$(BUILD_DDR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS_DDR) $(INC_FLAGS) -c $< -o $@

# -------------------------------------------------
# Compile rules (ASM) — DDR
# -------------------------------------------------
$(BUILD_DDR)/%.o: %.S
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS_DDR) $(INC_FLAGS) -c $< -o $@

# -------------------------------------------------
# Link (SRAM)
# -------------------------------------------------
$(BUILD)/$(TARGET).elf: $(OBJS) $(PORT_LIB) linker.ld | $(BUILD)
	$(LD) $(LDFLAGS) $(OBJS) $(PORT_LIB) -o $@

# -------------------------------------------------
# Link (DDR)
# -------------------------------------------------
$(BUILD_DDR)/$(TARGET_DDR).elf: $(OBJS_DDR) $(PORT_LIB) linker_ddr.ld | $(BUILD_DDR)
	$(LD) $(LDFLAGS_DDR) $(OBJS_DDR) $(PORT_LIB) -o $@

# -------------------------------------------------
# ELF -> raw binary (SRAM)
# -------------------------------------------------
$(BUILD)/$(TARGET).bin: $(BUILD)/$(TARGET).elf
	$(OBJCOPY) -O binary $< $@

# -------------------------------------------------
# ELF -> raw binary (DDR)
# -------------------------------------------------
$(BUILD_DDR)/$(TARGET_DDR).bin: $(BUILD_DDR)/$(TARGET_DDR).elf
	$(OBJCOPY) -O binary $< $@

# Disassembly listing (SRAM)
$(BUILD)/$(TARGET).lst: $(BUILD)/$(TARGET).elf
	@echo "OBJDUMP $@"
	$(OBJDUMP) -D $< > $@

# Disassembly listing (DDR)
$(BUILD_DDR)/$(TARGET_DDR).lst: $(BUILD_DDR)/$(TARGET_DDR).elf
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

size_ddr: $(BUILD_DDR)/$(TARGET_DDR).elf
	@echo ""
	@echo "=== Size Information (DDR) ==="
	@$(SIZE) $<
	@echo ""
	@echo "=== Memory Usage (DDR) ==="
	@SIZE=$$($(SIZE) -B $< | tail -1 | awk '{print $$1+$$2}'); \
	echo "Total (text+data): $$SIZE bytes"; \
	echo "Available DDR: 536870912 bytes (512MB)"; \
	PERCENT=$$(echo "scale=4; $$SIZE * 100 / 536870912" | bc); \
	echo "Usage: $$PERCENT%"

# -------------------------------------------------
# Cleanup
# -------------------------------------------------
clean:
	rm -rf common
	rm -rf $(BUILD)
	rm -rf $(BUILD_DDR)

.PHONY: all ddr_image clean size size_ddr