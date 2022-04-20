APP_NAME = userlib
OUT_DIR := ./out/
SRC_DIRS = ./src/
CFLAGS := -O3 -ffreestanding -nostdlib -mcpu=cortex-a53 -I./include/
SRCS = $(foreach dir,$(SRC_DIRS),$(shell find $(dir) -name '*.cpp' -or -name '*.c' -or -name '*.s'))
OBJS = $(SRCS:%=$(OUT_DIR)/%.o)

all: clean dirs $(APP_NAME)

dirs:
	mkdir -p $(OUT_DIR)

$(OUT_DIR)/%.s.o: %.s
	mkdir -p $(dir $@)
	clang --target=aarch64-elf $(CFLAGS) -c $< -o $@

$(OUT_DIR)/%.c.o: %.c
	mkdir -p $(dir $@)
	clang --target=aarch64-elf $(CFLAGS) -c $< -o $@

$(APP_NAME): dirs $(OBJS)

clean:
	rm -rf $(OUT_DIR) >/dev/null 2>/dev/null || true
