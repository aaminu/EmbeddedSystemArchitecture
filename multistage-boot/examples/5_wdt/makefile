# Toolchain Configuration
CROSS_COMPILE:=arm-none-eabi-
CC:=$(CROSS_COMPILE)gcc
LD:=$(CROSS_COMPILE)ld
OBJCOPY:=$(CROSS_COMPILE)objcopy
GDB:=$(CROSS_COMPILE)gdb

# Compiler and Linker Files
CFLAGS:=-mthumb -mcpu=cortex-m4 -g -ggdb -Wall -Wno-main -Wstack-usage=200 -ffreestanding -Wno-unused -DOWN_MALLOC #-DFPU_USE #-DSTACK_PAINTING
CFLAGS+=-I ../../includes
LDFLAGS:=-gc-sections -nostdlib

# Target executable
TARGET_EXEC:=image.bin

# Directory Structures
BUILD_DIR:=../../build
SRC_DIR:=../../src
BOOTLOADER_DIR:=../../bootloader
# OpenOCD file
DEBUG_FILE:=../../debug.cfg
INSTALL_FILE:=../../flash.cfg
#gdb file
GDB_SCRIPT = ./gdb_script.gdb


# Source Files
SRCS:=$(shell find $(SRC_DIR) -name '*.c' -or -name '*.s')
MAIN:=main

#Object Files: $(text:pattern=replacement)
OBJS:=$(SRCS:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)
OBJS+= $(BUILD_DIR)/$(MAIN).o

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
	$(OBJCOPY) -O binary $(^) $(@)
	@xxd $(@) $(@).txt

$(BUILD_DIR)/$(APP).bin: $(BUILD_DIR)/$(APP).elf
	@mkdir -p $(dir $@)
	$(OBJCOPY) -O binary $(^) $(@)
	@xxd $(@) $(@).txt


# Build ElF Files
$(BUILD_DIR)/$(BOOTLOADER).elf: $(BOOTLOADER_DIR)/$(BOOTLOADER).o $(BOOTLOADER_DIR)/$(BOOTLOADER).ld
	@mkdir -p $(dir $@)
	$(LD) $(<) -o $(@) -Map $(BUILD_DIR)/$(BOOTLOADER).map -T $(lastword $(^)) $(LDFLAGS)


$(BUILD_DIR)/$(APP).elf: $(sort $(OBJS)) $(APP).ld
	@mkdir -p $(dir $@)
	$(LD) $(wordlist 2, $(words $(OBJS)), $(^)) -o $(@) -Map $(BUILD_DIR)/$(APP).map -T $(lastword $(^)) $(LDFLAGS)


# Compile C source files to object files

# Rule for Bootloader
$(BOOTLOADER_DIR)/$(BOOTLOADER).o: $(BOOTLOADER_DIR)/$(BOOTLOADER).c
	$(CC) $(CFLAGS) -c $(<) -o $(@)

# Rule for Source Library Files 
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $(<) -o $(@)

# Rule for Main Application File
$(BUILD_DIR)/$(MAIN).o: $(MAIN).c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $(<) -o $(@)

# remove all intermediates
.INTERMEDIATE: $(OBJS)


#flash
.PHONY: flash
flash:
	@openocd -f $(INSTALL_FILE)

#flash
.PHONY: debug
debug:
	@openocd -f $(DEBUG_FILE)

#flash
.PHONY: gdb
gdb:
	@echo "target extended-remote localhost:3333" > $(GDB_SCRIPT)
	@echo "load" >> $(GDB_SCRIPT)
	@echo "add-symbol-file $(BUILD_DIR)/$(APP).elf 0x8004000" >> $(GDB_SCRIPT)
	@echo "echo Y\n b main" >> $(GDB_SCRIPT)
	@echo "continue" >> $(GDB_SCRIPT)
	$(GDB) -q $(BUILD_DIR)/$(BOOTLOADER).elf -x $(GDB_SCRIPT)
	@rm -f $(GDB_SCRIPT)


#Clean
.PHONY: clean
clean:
	-rm -r $(BUILD_DIR)
