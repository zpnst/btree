# DIRS
BIN_DIR = bin
SRC_DIR = src

TEST_DIR = $(SRC_DIR)/tests
VISUALIZE_DIR = $(SRC_DIR)/visualize
INCLUDES_DIR = $(SRC_DIR)/includes

# TARGETS
TARGET = btree
DEBUG_TARGET = debug_btree

# SRC FILES
SRCS = $(wildcard $(SRC_DIR)/*.c) $(wildcard $(INCLUDES_DIR)/*.c) $(wildcard $(VISUALIZE_DIR)/*.c) main.c

# COMPILATION
CC = clang
CFLAGS = -Wall -Wextra -I$(SRC_DIR) -I$(INCLUDES_DIR) -I$(TEST_DIR)
DEBUG_FLAGS = -g3 -ggdb
LDFLAGS = -lm

# DEFAULT TARGET
all: $(BIN_DIR)/$(TARGET)

# BTREE COMPILATION TARGET
$(BIN_DIR)/$(TARGET): $(SRCS)
	@mkdir -p $(BIN_DIR)
	@$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)
	echo "Built target: $@"

# DEBUG TARGETS
debug: CFLAGS += $(DEBUG_FLAGS)
debug: clean $(BIN_DIR)/$(DEBUG_TARGET)

# DEBUG BTREE COMPILATION TARGET
$(BIN_DIR)/$(DEBUG_TARGET): $(SRCS)
	@mkdir -p $(BIN_DIR)
	@$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)
	echo "Built debug target: $@"

# PHONY RUN TARGETS
run: all
	@./$(BIN_DIR)/$(TARGET)

run_debug:
	@cgdb ./$(BIN_DIR)/$(DEBUG_TARGET)

# CLEAN TARGET
clean:
	@rm -rf $(BIN_DIR)
	@find . -name "*.o" -delete
	@echo "Cleaned up build files."

# PHONY TARGETS DECLARATION
.PHONY: all clean run debug test run_debug
