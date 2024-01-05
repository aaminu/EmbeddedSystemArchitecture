target extended-remote localhost:3333
load
add-symbol-file ./build/app.elf 0x8004000
echo Y
 b main
continue
