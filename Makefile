BUILD_DIR := ./build/
OUT_DIR = ./out/
SRC_DIR = ./src/
INCLUDE_DIR = ./include/
USERLIB_DIR := ./userspace/userlib/
APPS_DIR := ./userspace/apps/
CFLAGS := -O3 -ffreestanding -nostdlib -mcpu=cortex-a53
SRCS = $(shell find $(SRC_DIR) -name '*.cpp' -or -name '*.c' -or -name '*.s')
OBJS = $(SRCS:%=$(BUILD_DIR)/%.o)
APPS = $(shell ls -1 $(APPS_DIR))
RAMDISK := ramdisk.img
RAMDISK_DIR := ./ramdisk_pack/
OUT_FILE_NAME = kernel8
OUT_FILE_IMG = $(OUT_DIR)/$(OUT_FILE_NAME).img
OUT_FILE_ELF = $(OUT_DIR)/$(OUT_FILE_NAME).elf
RAMDISK_SIZE := 67108864

all: clean dirs kernel8.img

ramdisk_dir:
	mkdir -p $(RAMDISK_DIR)

userlib: ramdisk_dir
	$(MAKE) -C $(USERLIB_DIR)

apps: ramdisk_dir userlib
	$(foreach app,$(APPS),$(shell $(MAKE) -C $(APPS_DIR)/$(app) OUT_DIR:=$(shell pwd)/$(RAMDISK_DIR)))

ramdisk: ramdisk_dir
	$(MAKE) -C ./FatFsMaker/
	./FatFsMaker/out/fatfs_maker $(RAMDISK_SIZE) $(RAMDISK) $(RAMDISK_DIR)

dirs:
	mkdir -p $(BUILD_DIR)
	mkdir -p $(OUT_DIR)

$(BUILD_DIR)/%.s.o: %.s
	mkdir -p $(dir $@)
	clang -I$(INCLUDE_DIR) --target=aarch64-elf $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/%.c.o: %.c
	mkdir -p $(dir $@)
	clang -I$(INCLUDE_DIR) --target=aarch64-elf $(CFLAGS) -c $< -o $@

kernel8.img: dirs $(OBJS)
	ld.lld -m aarch64elf -nostdlib $(OBJS) -T link.ld -o $(OUT_FILE_ELF)
	llvm-objcopy -O binary $(OUT_DIR)/kernel8.elf $(OUT_FILE_IMG)
	printf "%s" "RDRDRDRD" >> $(OUT_FILE_IMG)
	stat -c%s $(RAMDISK) | xargs printf "%016lx" | xxd -r -p >> $(OUT_FILE_IMG)
	dd bs=1 count=496 if=/dev/zero >> $(OUT_FILE_IMG)
	cat $(RAMDISK) >> $(OUT_FILE_IMG)

clean:
	rm -rf $(OUT_DIR) $(BUILD_DIR) $(RAMDISK_DIR) >/dev/null 2>/dev/null || true
	$(MAKE) -C $(USERLIB_DIR) clean
	$(foreach app,$(APPS),$(shell $(MAKE) -C $(APPS_DIR)/$(app) OUT_DIR:=$(shell pwd)/$(RAMDISK_DIR) clean))

run:
	qemu-system-aarch64 -M raspi3b -kernel $(OUT_DIR)/kernel8.img -serial stdio

debug:
	qemu-system-aarch64 -M raspi3b -kernel $(OUT_DIR)/kernel8.img -serial stdio -S -s

gdb:
	gdb-multiarch -ex "target remote 127.0.0.1:1234" -ex "layout asm"
