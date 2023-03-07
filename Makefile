ARCH = armv7-a
MCPU = cortex-a8

CC = arm-none-eabi-gcc
AS = arm-none-eabi-as
LD = arm-none-eabi-ld
OC = arm-none-eabi-objcopy

LINKER_SCRIPT = ./myFirstRTOS.ld

ASM_SRCS = $(wildcard boot/*.S)
ASM_OBJS = $(patsubst boot/%.S, build/%.o, $(ASM_SRCS))

myFirstRTOS = build/myFirstRTOS.axf
myFirstRTOS_bin = build/myFirstRTOS.bin

.PHONY: all clean run debug gdb

all: $(myFirstRTOS)

clean:
	@rm -fr build

run:
	qemu-system-arm -M realview-pb-a8 -kernel $(myFirstRTOS)

debug: $(myFirstRTOS)
	qemu-system-arm -M realview-pb-a8 -kernel $(myFirstRTOS) -S -gdb tcp::1234,ipv4

gdb:
	arm-none-eabi-gdb

$(myFirstRTOS): $(ASM_OBJS) $(LINKER_SCRIPT)
	$(LD) -n -T $(LINKER_SCRIPT) -o $(myFirstRTOS) $(ASM_OBJS)
	$(OC) -O binary $(myFirstRTOS) $(myFirstRTOS_bin)

build/%.o: boot/%.S
	mkdir -p $(shell dirname $@)
	$(AS) -march=$(ARCH) -mcpu=$(MCPU) -g -o $@ $<
