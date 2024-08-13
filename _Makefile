CC = gcc
CC_i686 = i686-w64-mingw32-gcc
CC_x86_64 = x86_64-w64-mingw32-gcc

CFLAGS = -std=c99 -Wpedantic -Wextra -Werror -Wall -Wstrict-aliasing=3
CFLAGS += -Wwrite-strings -Wvla -Wcast-align=strict -Wstrict-prototypes
CFLAGS += -Wstringop-overflow=4 -Wshadow -fanalyzer

AUTOFMT = clang-format
STYLE=GNU

SRC_DIR = src
INCL_DIR = include

# Only used in `test`
DAT_DIR = dat-files

MAIN_FILE = $(SRC_DIR)/main.c
MAIN_OBJ = $(SRC_DIR)/main.o
SRC_FILES = $(filter-out $(MAIN_FILE), $(wildcard $(SRC_DIR)/*.c))
OBJ_FILES = $(SRC_FILES:$(SRC_DIR)/%.c=$(SRC_DIR)/%.o)

TARGET = HarvesterHarvester
LIBRARY = bin/lib/HarvesterHarvester/libhh.a

OUTPUT = output/*

all: clean format $(TARGET)

full: all test

release: clean format $(TARGET) WIN32 WIN64 library docs copy

$(SRC_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@ -I./$(INCL_DIR)/

$(MAIN_OBJ): $(MAIN_FILE)
	$(CC) $(CFLAGS) -c $< -o $@ -I./$(INCL_DIR)/

$(TARGET): $(OBJ_FILES) $(MAIN_OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ_FILES) $(MAIN_OBJ) -I./$(INCL_DIR)/

WIN64: CC = $(CC_x86_64)
WIN64: $(OBJ_FILES) $(MAIN_OBJ)
	$(CC_x86_64) $(CFLAGS) -o $(TARGET)_x86_64.exe $(OBJ_FILES) $(MAIN_OBJ) -I./$(INCL_DIR)

WIN32: CC = $(CC_i686)
WIN32: $(OBJ_FILES) $(MAIN_OBJ)
	$(CC_i686) $(CFLAGS) -o $(TARGET)_i686.exe $(OBJ_FILES) $(MAIN_OBJ) -I./$(INCL_DIR)

library: $(OBJ_FILES)
	ar rcs $(LIBRARY) $(OBJ_FILES)

copy:
	cp -r $(INCL_DIR)/*.h ./bin/include/HarvesterHarvester/

test: $(TARGET)
	valgrind --leak-check=yes ./$(TARGET) $(DAT_DIR)/HARVEST.DAT

clean:
	-@rm $(LIBRARY) 2>/dev/null || true
	-@rm ./bin/include/HarvesterHarvester/ 2>/dev/null || true
	-@rm $(SRC_DIR)/*.o 2>/dev/null || true
	-@rm $(TARGET) 2>/dev/null || true
	-@rm $(TARGET)_i686.exe 2>/dev/null || true
	-@rm $(TARGET)_x86_64.exe 2>/dev/null || true
	-@rm -rf $(OUTPUT) 2>/dev/null || true

format: 
	$(AUTOFMT) -style=$(STYLE) -i $(SRC_FILES) $(INCL_DIR)/*.h

docs:
	doxygen docs/doxygen-config

.PHONY: docs clean $(OBJ_FILES) $(MAIN_OBJ) $(TARGET) WIN32 WIN64
