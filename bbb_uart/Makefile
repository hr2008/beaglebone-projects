# -------------------------------------------------
# Toolchain
# -------------------------------------------------
CROSS   = arm-none-eabi
CC      = $(CROSS)-gcc
LD      = $(CROSS)-ld
OBJCOPY = $(CROSS)-objcopy

# -------------------------------------------------
# Target
# -------------------------------------------------
TARGET  = enable_uart
BUILD   = build

# -------------------------------------------------
# Flags
# -------------------------------------------------
CFLAGS  = -mcpu=cortex-a8 -marm -O2 \
          -ffreestanding -nostdlib -nostartfiles \
          -fno-pic -fno-pie

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
all: $(BUILD)/$(TARGET).bin

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

# -------------------------------------------------
# Cleanup
# -------------------------------------------------
clean:
	rm -rf common
	rm -rf $(BUILD)

.PHONY: all clean
