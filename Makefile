TARGET = main
BIN_DIR = bin
SRC_DIR = src

SRCS = $(SRC_DIR)/btree.c main.c

all:
	@mkdir -p $(BIN_DIR)  
	@clang -lm $(SRCS) -o $(BIN_DIR)/$(TARGET)

run: all
	@./$(BIN_DIR)/$(TARGET)

clean:
	@rm -rf $(BIN_DIR)

.PHONY: all clean run
