# Arm Complier
CC=arm-none-eabi-gcc
CFLAGS=-mthumb -mcpu=cortex-m4 -ffreestanding

# Files
OBJS += hello.o 
OBJS += world.o 


# $(@) ----> Name of target for the current execution rule
# $(^) ----> List all prerequisites with out duplication
# $(+) ----> List all prerequisites with duplication
# $(<)  ----> First element of the prerequisite list
helloworld: $(OBJS)
	arm-none-eabi-ld -o image.elf $(^) -T link.ld -Map=map_file.map
	arm-none-eabi-objcopy -O binary image.elf image.bin


clean:
	rm *.o *.map *.elf *.bin
