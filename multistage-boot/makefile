# Toolchain Configuration
CROSS_COMPILE:=arm-none-eabi-
CC:=$(CROSS_COMPILE)gcc
LD:=$(CROSS_COMPILE)ld
OBJCOPY:=$(CROSS_COMPILE)objcopy

# Compiler and Linker Files
CFLAGS:=-mthumb -mcpu=cortex-m3 -g -ggdb -Wall -Wno-main -Wstack-usage=200 -ffreestanding -Wno-unused -DOWN_MALLOC #-DSTACK_PAINTING
LDFLAGS:=-gc-sections -nostdlib 

# Target executable
TARGET_EXEC:=image.bin

# Directory Structures
BUILD_DIR:=./build
SRC_DIR:=./src

# OpenOCD file
DEBUG_FILE:=./debug.cfg
INSTALL_FILE:=./flash.cfg

# Source Files
SRCS:=$(shell find $(SRC_DIR) -name '*.c' -or -name '*.s')

#Object Files: $(text:pattern=replacement)
OBJS:=$(SRCS:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)

# Main Build Targets
APP:= app
BOOTLOADER:= _bootloader

# Binary Files
BINARY:=$(BUILD_DIR)/$(BOOTLOADER).bin $(BUILD_DIR)/$(APP).bin



# $(@) ----> Name of target for the current execution rule
# $(^) ----> List all prerequisites with out duplication
# $(+) ----> List all prerequisites with duplication
# $(<) ----> First element of the prerequisite list
# $(?) ----> Outputs all prerequisites newer than the target

# Link Final Target
$(BUILD_DIR)/$(TARGET_EXEC): $(sort $(BINARY))
	@mkdir -p $(dir $@)
	@ls $(^)
# 	cat $(^) > $(@)

# Build Binary Files
$(BUILD_DIR)/$(BOOTLOADER).bin: $(BUILD_DIR)/$(BOOTLOADER).elf
	@mkdir -p $(dir $@)
#	$(OBJCOPY) -O binary --pad-to=0x010 --gap-fill=0xEA $(^) $(@)
	$(OBJCOPY) -O binary $(^) $(@)
	@xxd $(@) $(@).txt

$(BUILD_DIR)/$(APP).bin: $(BUILD_DIR)/$(APP).elf
	@mkdir -p $(dir $@)
	$(OBJCOPY) -O binary $(^) $(@)
	@xxd $(@) $(@).txt


# Build ElF Files
$(BUILD_DIR)/$(BOOTLOADER).elf: $(firstword $(sort $(OBJS))) $(BOOTLOADER).ld
	@mkdir -p $(dir $@)
	$(LD) $(LDFLAGS)  $(<) -o $(@)  -Map $(BUILD_DIR)/$(BOOTLOADER).map -T $(lastword $(^))


$(BUILD_DIR)/$(APP).elf: $(sort $(OBJS)) $(APP).ld
	@mkdir -p $(dir $@)
	$(LD) $(LDFLAGS)  $(wordlist 2, $(words $(OBJS)), $(^)) -o $(@)  -Map $(BUILD_DIR)/$(APP).map -T $(lastword $(^))


# Compile C source files to object files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $(<) -o $(@) 


#flash
.PHONY: flash
flash:
	@openocd -f $(INSTALL_FILE)

#flash
.PHONY: debug
debug:
	@openocd -f $(DEBUG_FILE)

#Clean
.PHONY: clean
clean:
	-rm -r $(BUILD_DIR)
