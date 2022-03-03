SRCS = $(wildcard *.c) $(wildcard *.s)
OBJS = $(SRCS:.c=.c_o) $(SRCS:.s=.s_o)
BUILD_DIR = "build/"
OUT_DIR = "out/"
CFLAGS = -O3 -ffreestanding -nostdlib -mcpu=cortex-a53+nosimd -Iinclude

all: clean dirs kernel8.img

dirs:
	mkdir -p $(BUILD_DIR)
	mkdir -p $(OUT_DIR)

%.s_o: %.s
	clang --target=aarch64-elf $(CFLAGS) -c $< -o $(BUILD_DIR)/$@

%.c_o: %.c
	clang --target=aarch64-elf $(CFLAGS) -c $< -o $(BUILD_DIR)/$@

kernel8.img: dirs $(OBJS)
	ld.lld -m aarch64elf -nostdlib $(BUILD_DIR)/*.c_o $(BUILD_DIR)/*.s_o -T link.ld -o $(OUT_DIR)/kernel8.elf
	llvm-objcopy -O binary $(OUT_DIR)/kernel8.elf $(OUT_DIR)/kernel8.img

clean:
	rm -rf $(OUT_DIR) $(BUILD_DIR) >/dev/null 2>/dev/null || true

run:
	qemu-system-aarch64 -M raspi3b -kernel $(OUT_DIR)/kernel8.elf -serial stdio

debug:
	qemu-system-aarch64 -M raspi3b -kernel $(OUT_DIR)/kernel8.elf -serial stdio -S -s

gdb:
	gdb-multiarch -ex "target remote 127.0.0.1:1234" -ex "layout asm"
