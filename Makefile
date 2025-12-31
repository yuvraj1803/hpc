CC ?= clang
CFLAGS ?= -Wall -O0
SRC_DIR := src
OUT_DIR := out

SOURCES := $(shell find $(SRC_DIR) -name '*.c')
TARGETS := $(patsubst $(SRC_DIR)/%.c,$(OUT_DIR)/%,$(SOURCES))

all: $(TARGETS)

$(OUT_DIR)/%: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -rf $(OUT_DIR)

list:
	@echo $(TARGETS)

