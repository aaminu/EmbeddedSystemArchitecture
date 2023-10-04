CROSS_COMPILE:=arm-none-eabi-
CC:=$(CROSS_COMPILE)gcc
LD:=$(CROSS_COMPILE)ld
OBJCOPY:=$(CROSS_COMPILE)objcopy

CFLAGS:=-mthumb -mcpu=cortex-m3 -g -ggdb -Wall -Wno-main
LDFLAGS:=-T target.ld -gc-sections -nostdlib -Map=image.map

# $(@) ----> Name of target for the current execution rule
# $(^) ----> List all prerequisites with out duplication
# $(+) ----> List all prerequisites with duplication
# $(<)  ----> First element of the prerequisite list


image.bin: image.elf
	$(OBJCOPY) -O binary $(^) $(@)

image.elf: startup.o target.ld
	$(LD) $(LDFLAGS)  $(<) -o $(@)

startup.o: startup.c

clean:
	rm -f image.bin image.elf *.o image.map
