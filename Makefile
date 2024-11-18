# DIRS
BIN_DIR = bin
SRC_DIR = src
INCLUDES_DIR = $(SRC_DIR)/includes
TEST_DIR = $(SRC_DIR)/tests

# TARGETS
TARGET = btree
DEBUG_TARGET = debug_btree
TEST_TARGET = test_btree

# SRC FILES
SRCS = $(wildcard $(SRC_DIR)/*.c) $(wildcard $(INCLUDES_DIR)/*.c) main.c
TEST_SRCS = $(wildcard $(TEST_DIR)/*.c) $(SRCS) main.c

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

# TEST TARGET
test: $(BIN_DIR)/$(TEST_TARGET)

# TEST BTREE COMPILATION TARGET (ALSO main.c, BUT LINKED WITH test.c)
$(BIN_DIR)/$(TEST_TARGET): $(TEST_SRCS)
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)
	@echo "Built test target: $@"

# PHONY RUN TARGETS
run: all
	@./$(BIN_DIR)/$(TARGET)

run_test: test
	@./$(BIN_DIR)/$(TEST_TARGET)

run_debug:
	@cgdb ./$(BIN_DIR)/$(DEBUG_TARGET)

# CLEAN TARGET
clean:
	@rm -rf $(BIN_DIR)
	@find . -name "*.o" -delete
	@echo "Cleaned up build files."

# PHONY TARGETS DECLARATION
.PHONY: all clean run debug test run_test run_debug
