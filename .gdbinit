set auto-load local-gdbinit
target extended :4242
monitor reset halt
file build/ch.elf
load build/ch.elf
b main
continue
