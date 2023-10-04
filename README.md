## Using OpenOCD
To start openocd
```bash
openocd -f <filepath to .cfg>
```
openocd path is located at  `/usr/share/openocd/`


Connect to the telnet port
```bash
telnet localhost <port specified>
```

Probing the device by
```bash
> init
> halt
> flash probe 0
> flash info 0
> flash 0 0 <number of sector>
> flash write_image <path/to/image.bin> <start address>
```
Connecting to the debug server provided by openocd using the arm-none-eabi-gdb tool
```bash
> arm-none-eabi-gdb
(gdb) target extended-remote localhost:3333
```
Debug a system using Qemu
```
qemu-system-arm -machine <machine type> --kernel <image binary> -nographic -S -gdb tcp:3333
```

Checking if the Makefile is properly formatted with tabs and line endings, do `cat -e -t -v <makefile_name>`. The output will have $ denoting line endings and ^I indicating tab.
