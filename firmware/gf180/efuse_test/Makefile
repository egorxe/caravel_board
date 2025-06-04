# Set the prefix for `riscvXX-unknown-elf-*`
# On installations using `multilib`, this will be `riscv64` even for compiling to 32-bit targets
TOOLCHAIN_PREFIX?=riscv64-unknown-elf

.PHONY: hex flash clean

PATTERN = efuse_test

hex:  ${PATTERN:=.hex}

%.elf: %.c ../sections.lds ../crt0_vex.S
	$(TOOLCHAIN_PATH)$(TOOLCHAIN_PREFIX)-gcc -I../ -I../generated/ -O0 -mabi=ilp32 -march=rv32i_zicsr -D__vexriscv__ -Wl,-Bstatic,-T,../sections.lds,--strip-debug -ffreestanding -nostdlib -o $@ ../crt0_vex.S ../isr.c $<

%.hex: %.elf
	$(TOOLCHAIN_PATH)$(TOOLCHAIN_PREFIX)-objcopy -O verilog $< $@
	sed -i 's/@1000/@0000/g' $@

%.bin: %.elf
	$(TOOLCHAIN_PATH)$(TOOLCHAIN_PREFIX)-objcopy -O binary $< $@
	
%.lst: %.elf
	${TOOLCHAIN_PATH}$(TOOLCHAIN_PREFIX)-objdump -D  $(PATTERN).elf >  $(PATTERN).lst

flash:  $(PATTERN).hex
	python3 ../util/caravel_hkflash.py  $(PATTERN).hex

flash2:  $(PATTERN).hex
	python3 ../util/caravel_flash.py  $(PATTERN).hex

# ---- Clean ----

clean:
	rm -f *.elf *.hex *.bin *.vvp *.vcd *.lst

.PHONY: clean hex all flash

