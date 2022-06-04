BUILD_DIR := ./build/
OUT_DIR := ./out/
LD_SCRIPT = ../../link.ld
USERLIB_PATH := ../../userlib/
USERLIB_OBJS_DIR = $(USERLIB_PATH)/out/
SRC_DIRS = ./src/
CFLAGS := -O3 -Wall -g -fno-common -fno-PIE -ffreestanding -fpic -ffunction-sections -fdata-sections -isystem $(shell $(CC) -print-file-name=include) -isystem sysinc -fno-stack-protector -mgeneral-regs-only -mstrict-align
LDFLAGS := -EL -maarch64elf --no-undefined -X -Bsymbolic -z notext --no-apply-dynamic-relocs -z nocopyreloc --gc-sections -pie -nostdlib
SRCS = $(shell find $(SRC_DIRS) -name '*.cpp' -or -name '*.c' -or -name '*.s')
USERLIB_OBJS = $(shell find $(USERLIB_OBJS_DIR) -name '*.o')
OBJS = $(SRCS:%=$(BUILD_DIR)/%.o)
OUT_FILE_RAW = $(OUT_DIR)/$(APP_NAME)
OUT_FILE_ELF = $(BUILD_DIR)/$(APP_NAME).elf

all: clean dirs $(APP_NAME)

dirs:
	mkdir -p $(BUILD_DIR)
	mkdir -p $(OUT_DIR)

$(BUILD_DIR)/%.s.o: %.s
	mkdir -p $(dir $@)
	clang --target=aarch64-elf $(CFLAGS) -I./include/ -I$(USERLIB_PATH)/include/ -c $< -o $@

$(BUILD_DIR)/%.c.o: %.c
	mkdir -p $(dir $@)
	clang --target=aarch64-elf $(CFLAGS) -I./include/ -I$(USERLIB_PATH)/include/ -c $< -o $@

$(APP_NAME): dirs $(OBJS)
	ld.lld $(LDFLAGS) -L$(USERLIB_PATH)/out/ $(OBJS) $(USERLIB_OBJS) -T ../../link.ld -o $(OUT_FILE_ELF)
	llvm-objcopy -O binary $(OUT_FILE_ELF) $(OUT_FILE_RAW)

clean:
	rm -rf $(BUILD_DIR) >/dev/null 2>/dev/null || true
