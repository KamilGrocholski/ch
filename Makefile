debug?=0
NAME=main
SRC_DIR=src
BUILD_DIR=build
INCLUDE_DIR=src
LIB_DIR=lib
TESTS_DIR=test
BIN_DIR=bin

ENTRYCFILE=$(SRC_DIR)/$(NAME).c
SRCCFILES=$(shell find src -type f -name "*.c" ! -name "$(NAME).c")
TESTCFILES=$(shell find test -type f -name "*.c")

OBJS=$(patsubst %.c,%.o, $(SRCCFILES) $(wildcard $(LIB_DIR)/**/*.c))

CC=gcc
LINTER=
FORMATTER=

CFLAGS=-std=gnu17 -D _GNU_SOURCE -D __STDC_WANT_LIB_EXT1__ -Wall -Wextra -pedantic -I$(INCLUDE_DIR)
LDFLAGS=

ifeq ($(debug), 1)
	CFLAGS:=$(CFLAGS) -g -O0
else
	CFLAGS:=$(CFLAGS) -Oz
endif

$(NAME): dir $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $(BIN_DIR)/$@ $(patsubst %, build/%, $(OBJS)) $(ENTRYCFILE) 

$(OBJS): dir
	@mkdir -p $(BUILD_DIR)/$(@D)
	@$(CC) $(CFLAGS) -o $(BUILD_DIR)/$@ -c $*.c

test: $(OBJS)
	@$(CC) $(CFLAGS) -o $(BIN_DIR)/$(NAME)_test $(TESTCFILES) $(patsubst %, build/%, $^)
	@$(BIN_DIR)/$(NAME)_test

check: $(NAME)
	valgrind -s --leak-check=full --show-leak-kinds=all $(BIN_DIR)/$<

setup:

dir:
	@mkdir -p $(BUILD_DIR) $(BIN_DIR)

clean:
	@rm -rf $(BUILD_DIR) $(BIN_DIR)

.PHONY: check setup dir clean
