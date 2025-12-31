CC ?= clang
CFLAGS ?= -Wall -O0 -I./tools

SRC_DIRS := src stress
OUT_DIR  := out

SOURCES := $(shell find $(SRC_DIRS) -name '*.c')
TARGETS := $(patsubst %.c,$(OUT_DIR)/%,$(SOURCES))

all: $(TARGETS)

$(OUT_DIR)/%: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -rf $(OUT_DIR)
	find . -type f -name "*.swp" -delete

list:
	@echo $(TARGETS)

install:
	pip3 install --break-system-packages -r requirements.txt
