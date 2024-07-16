CC=gcc
SRC_DIR=src
DAT_DIR=dat\ files
SRC_FILES=$(SRC_DIR)/*.c
INCL_FILES=$(SRC_DIR)/*.h
LIBS=
TARGET=main
OUTPUT=output/*
CFLAGS=-std=c99 -Wpedantic -Wextra -Werror -Wall -Wstrict-aliasing=3 -Wwrite-strings -Wvla -Wcast-align=strict -Wstrict-prototypes -Wstringop-overflow=4 -Wshadow -fanalyzer
STYLE=GNU

all: clean format $(TARGET)

full: all test

$(TARGET): 
	$(CC) -o $(TARGET) $(SRC_FILES) $(TEST_SRC_FILES) $(CFLAGS) -I./$(INCL_FILES)

test: $(TARGET)
	valgrind --leak-check=yes ./$(TARGET) $(DAT_DIR)/HARVEST.DAT --debug

clean:
	-@rm $(TARGET) 2>/dev/null || true
	-@rm -rf $(OUTPUT) 2>/dev/null || true

format: 
	clang-format -style=$(STYLE) -i $(SRC_FILES) $(INCL_FILES)
