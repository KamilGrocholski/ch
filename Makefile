CC = gcc
CFLAGS = -Wall -I./src

SRC_DIR = src
OBJ_DIR = build
TEST_DIR = test

SRC = $(SRC_DIR)/main.c \
      $(SRC_DIR)/core/array.c \
      $(SRC_DIR)/core/memory.c \
      $(SRC_DIR)/core/string.c \
      $(SRC_DIR)/core/str.c \
      $(SRC_DIR)/core/arena.c \
      $(SRC_DIR)/core/logger.c \
      $(SRC_DIR)/fs/fs.c

OBJ = $(SRC:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

TEST = $(TEST_DIR)/test_manager.c \
       $(TEST_DIR)/core/arena_test.c	

TARGET = main
TEST_TARGET = test_main

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)/core
	@mkdir -p $(OBJ_DIR)/fs
	$(CC) $(CFLAGS) -c $< -o $@

test: $(OBJ)
	$(CC) $(CFLAGS) -o $(TEST_TARGET) $(TEST) $(OBJ)
	./$(TEST_TARGET)

clean:
	rm -f $(TARGET) $(TEST_TARGET) $(OBJ)
	rm -rf $(OBJ_DIR)

valgrind: $(TARGET)
	valgrind --leak-check=yes ./$(TARGET)

