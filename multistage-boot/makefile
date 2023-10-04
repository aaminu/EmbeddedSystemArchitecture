CROSS_COMPILE:=arm-none-eabi-
CC:=$(CROSS_COMPILE)gcc
LD:=$(CROSS_COMPILE)ld
OBJCOPY:=$(CROSS_COMPILE)objcopy

CFLAGS:=-mthumb -mcpu=cortex-m3 -g -ggdb -Wall -Wno-main
LDFLAGS:=-gc-sections -nostdlib 

TARGET_EXEC:=image.bin

# DIRECTORY
BUILD_DIR:=./build
SRC_DIR:=./src

# Source Files
SRCS:=$(shell find $(SRC_DIR) -name '*.c' -or -name '*.s')

#Object Files: $(text:pattern=replacement)
OBJS:=$(SRCS:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)
ELFS:=$(OBJS:.o=.elf)

#BUILD OBJECTS.
BINARY:=$(SRCS:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.bin)

# $(@) ----> Name of target for the current execution rule
# $(^) ----> List all prerequisites with out duplication
# $(+) ----> List all prerequisites with duplication
# $(<) ----> First element of the prerequisite list
# $(?) ----> Outputs all prerequisites newer than the target

$(BUILD_DIR)/$(TARGET_EXEC): $(sort $(BINARY))
	@mkdir -p $(dir $@)
	@ls $(^)
	cat $(^) > $(@)

$(BUILD_DIR)/%.bin: $(BUILD_DIR)/%.elf
	@mkdir -p $(dir $@)
ifeq ($*, $(filter _bootloader,$*))
	$(OBJCOPY) -O binary --pad-to=0x001000 --gap-fill=0xFF $(^) $(@)
else
	$(OBJCOPY) -O binary $(^) $(@)
endif
	
$(ELFS): $(BUILD_DIR)/%.elf: $(BUILD_DIR)/%.o ./%.ld
	@mkdir -p $(dir $@)
	$(LD) $(LDFLAGS)  $(<) -o $(@)  -Map $(patsubst %.elf, %.map, $(@)) -T $(word 2,$^)

# targets …: target-pattern: prereq-patterns …
$(OBJS): $(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $(<) -o $(@) 

.PHONY: clean
clean:
	-rm -r $(BUILD_DIR)
