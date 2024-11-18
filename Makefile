
BIN_DIR = bin
SRC_DIR = src

TARGET = btree
DEBUG_TARGET = debug_btree

SRCS = $(SRC_DIR)/btree.c main.c

all:
	@mkdir -p $(BIN_DIR)  
	@clang -lm $(SRCS) -o $(BIN_DIR)/$(TARGET)
	@clang -lm -ggdb -g3 $(SRCS) -o $(BIN_DIR)/$(DEBUG_TARGET)

run: all
	@./$(BIN_DIR)/$(TARGET)

clean:
	@rm -rf $(BIN_DIR)

debug:
	@cgdb ./$(BIN_DIR)/$(DEBUG_TARGET)

.PHONY: all clean run
