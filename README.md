## About
This repository contains my work on programming the STM32 Cortex M4 microcontroller from the ground up. Instead of using pre-built libraries like STM32 HAL or CMSIS, I'm manually setting up registers, configuring clocks, and developing my own hardware abstraction layer (HAL) to interact with peripherals. This project aims to gain a deeper understanding of the microcontroller's architecture and low-level operations, with a focus on performance optimization and control.

## **Usage**
### **Makefile**

### **Makefile**
Each folder has its own makefiles. One problem I encountered initially was my makefile not properly formatted, this can be checked by issuing `cat -e -t -v <makefile_name>`. The output will have $ denoting line endings and ^I indicating tab.

### **Debuging Using OpenOCD**
Debugging is done with the help of OpenOCD. Two scripts are provided:
- [flash.cfg](./multistage-boot/flash.cfg)
- [debug.cfg](./multistage-boot/debug.cfg)

The scripts above can be used in different scenarios listed above:
1. Changes to source files would required rebuilding and flashing the target. The script from above have been incorporated into the [makefile](./multistage-boot/makefile) and a typical process would look like.
    ```bash
    cd ESA/multistage-boot/examples/<directory>
    make clean
    make
    # Make sure your device is connected at this point
    make flash
    make debug
    ```
2. No changes but a new board to be flashed, the process would look like 
    ```bash
    cd ESA/multistage-boot/examples/<directory>
    # Make sure your device is connected at this point
    make flash
    make debug
    ```
    or 
    ```bash
    cd ESA/multistage-boot/
    openocd -f flash.cfg
    openocd -f debug.cfg
    ```
3. Simple debugging
    ```bash
    cd ESA/multistage-boot/
    openocd -f debug.cfg
    ```
Once the debug server is started and listening for connections on the tcp port, use the arm-none-eabi-gdb to connect. Since it is a multistage boot system, the symbol files for both bootloader and application are loaded individual:
```bash
arm-none-eabi-gdb -q /home/aaminu/ESA/multistage-boot/build/_bootloader.elf
(gdb) target extended-remote localhost:3333
(gdb) load
(gdb) add-symbol-file /home/aaminu/ESA/multistage-boot/build/app.elf 0x8004000
# Answer y to the prompt

# Set the breakpoint, example below
(gdb) b main
(gdb) continue

# After breakpoint is reached, I like to use the layout to see where in the sorce code I am
(gdb) layout src
# Step through as you wish inspecting symbols and registers
```

Furthermore, one can connect to the openocd server by using the following commands and inspect the target device:

    ```bash
    telnet localhost 4444
    > init
    > halt
    > flash probe 0
    > flash info 0
    > flash erase_sector 0 0 <number of sector>
    > flash write_image <path/to/image.bin> <start address>
    ```
Finally, openocd path is located at  `/usr/share/openocd/`

### Credits
- Embedded System Architecture - Dainele Lacamera
- [controllerstech](https://controllerstech.com/)
- STM32 Board Specifications and Documentations
