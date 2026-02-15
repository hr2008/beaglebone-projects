# -------------------------------------------------
# Toolchain
# -------------------------------------------------
CROSS   = arm-none-eabi
CC      = $(CROSS)-gcc
LD      = $(CROSS)-ld
OBJCOPY = $(CROSS)-objcopy
OBJDUMP = $(CROSS)-objdump
SIZE    = $(CROSS)-size

# -------------------------------------------------
# Target
# -------------------------------------------------
TARGET  = blink_uart
BUILD   = build

# CPU and architecture flags
CPU = cortex-a8
ARCH = armv7-a

# -------------------------------------------------
# Flags
# -------------------------------------------------
# Compiler flags
CFLAGS = -mcpu=$(CPU)
CFLAGS += -mfpu=neon -mfloat-abi=softfp
CFLAGS += -Wall -Wextra -Werror
CFLAGS += -O2 -g
CFLAGS += -ffreestanding -nostdlib -nostartfiles
CFLAGS += -fno-common -ffunction-sections -fdata-sections

LDFLAGS = -T linker.ld

# -------------------------------------------------
# Source discovery (recursive)
# -------------------------------------------------
C_SRCS := $(shell find ../common -name '*.c')
S_SRCS := $(shell find ../common -name '*.s')
C_SRCS += $(shell find . -name '*.c')
S_SRCS += $(shell find . -name '*.s')

# -------------------------------------------------
# Object files (mirrored into build/)
# -------------------------------------------------
OBJS := $(C_SRCS:%.c=$(BUILD)/%.o) \
        $(S_SRCS:%.s=$(BUILD)/%.o)

# -------------------------------------------------
# Auto include directories
# -------------------------------------------------
INC_DIRS := $(sort $(dir $(shell find ../common -name '*.h')))
INC_DIRS += $(sort $(dir $(shell find . -name '*.h')))
INC_FLAGS := $(addprefix -I,$(INC_DIRS))

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
$(BUILD)/%.o: %.s
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INC_FLAGS) -c $< -o $@

# -------------------------------------------------
# Link
# -------------------------------------------------
$(BUILD)/$(TARGET).elf: $(OBJS) linker.ld | $(BUILD)
	$(LD) $(LDFLAGS) $(OBJS) -o $@

# -------------------------------------------------
# ELF -> raw binary
# -------------------------------------------------
$(BUILD)/$(TARGET).bin: $(BUILD)/$(TARGET).elf
	$(OBJCOPY) -O binary $< $@

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
