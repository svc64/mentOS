SRCS = $(wildcard *.c)
OBJS = $(SRCS:.c=.o)
BUILD_DIR = "build/"
OUT_DIR = "out/"
CFLAGS = -O3 -ffreestanding -nostdlib -mcpu=cortex-a53+nosimd

all: clean dirs kernel8.img

dirs:
	mkdir -p $(BUILD_DIR)
	mkdir -p $(OUT_DIR)

start.o: dirs start.S
	clang --target=aarch64-elf $(CFLAGS) -c start.S -o $(BUILD_DIR)/start.o

%.o: %.c
	clang --target=aarch64-elf $(CFLAGS) -c $< -o $(BUILD_DIR)/$@

kernel8.img: dirs start.o $(OBJS)
	ld.lld -m aarch64elf -nostdlib $(BUILD_DIR)/*.o -T link.ld -o $(OUT_DIR)/kernel8.elf
	llvm-objcopy -O binary $(OUT_DIR)/kernel8.elf $(OUT_DIR)/kernel8.img

clean:
	rm -rf $(OUT_DIR) $(BUILD_DIR) >/dev/null 2>/dev/null || true

run:
	qemu-system-aarch64 -M raspi3b -kernel $(OUT_DIR)/kernel8.elf -serial stdio
