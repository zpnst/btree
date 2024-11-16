TARGET = main
SRCS = main.c btree.c
BIN_DIR = bin

all:
	@mkdir -p $(BIN_DIR)  
	@clang -lm $(SRCS) -o $(BIN_DIR)/$(TARGET)

run: all
	@./$(BIN_DIR)/$(TARGET)

clean:
	@rm -rf $(BIN_DIR)

.PHONY: all clean run
