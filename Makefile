debug ?= 0
NAME = main
SRC_DIR = src
BUILD_DIR = build
INCLUDE_DIR = src
LIB_DIR =
TESTS_DIR = test
BIN_DIR = bin

ENTRYCFILE = $(SRC_DIR)/$(NAME).c
SRCCFILES = $(shell find $(SRC_DIR) -type f -name "*.c" ! -name "$(NAME).c")
# LIBCFILES = $(shell find $(LIB_DIR) -type f -name "*.c")
TESTCFILES = $(shell find $(TESTS_DIR) -type f -name "*.c")

OBJS = $(patsubst %.c, $(BUILD_DIR)/%.o, $(SRCCFILES) $(LIBCFILES))

CC = gcc
CFLAGS = -std=gnu17 -D _GNU_SOURCE -D __STDC_WANT_LIB_EXT1__ -Wall -Wextra -pedantic -I$(INCLUDE_DIR)
LDFLAGS = -lm

ifeq ($(debug), 1)
	CFLAGS += -g -O0
else
	CFLAGS += -Oz
endif

$(NAME): dir $(OBJS)
	$(CC) $(CFLAGS) -o $(BIN_DIR)/$(NAME) $(OBJS) $(ENTRYCFILE) $(LDFLAGS)

$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

test: dir $(OBJS)
	$(CC) $(CFLAGS) -o $(BIN_DIR)/$(NAME)_test $(OBJS) $(TESTCFILES) $(LDFLAGS)
	@$(BIN_DIR)/$(NAME)_test

check: $(NAME)
	valgrind -s --track-origins=yes --leak-check=full --show-leak-kinds=all $(BIN_DIR)/$(NAME)

setup:

dir:
	@mkdir -p $(BUILD_DIR) $(BIN_DIR)

clean:
	@rm -rf $(BUILD_DIR) $(BIN_DIR)

.PHONY: check setup dir clean test
