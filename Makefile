CFLAGS = -std=c99 -Wpedantic -Wextra -Werror -Wall -Wstrict-aliasing=3
CFLAGS += -Wwrite-strings -Wvla -Wcast-align=strict -Wstrict-prototypes
CFLAGS += -Wstringop-overflow=4 -Wshadow -fanalyzer

SRC_DIR = src
DAT_DIR = dat-files

SRC_FILES = $(SRC_DIR)/*.c
INCL_FILES = $(SRC_DIR)/*.h

TARGET = HarvesterHarvester

OUTPUT = output/*

STYLE=GNU

all: clean format $(TARGET)

full: all test

.PHONY: docs

release: clean format $(TARGET) WIN32 WIN64 docs


$(TARGET):
	gcc -o $(TARGET) $(SRC_FILES) $(TEST_SRC_FILES) $(CFLAGS) -I./$(INCL_FILES)

WIN32:
	i686-w64-mingw32-gcc -o $(TARGET)_i686.exe $(SRC_FILES) $(TEST_SRC_FILES) $(CFLAGS) -I./$(INCL_FILES)

WIN64:
	x86_64-w64-mingw32-gcc -o $(TARGET)_x86_64.exe $(SRC_FILES) $(TEST_SRC_FILES) $(CFLAGS) -I./$(INCL_FILES)

test: $(TARGET)
	valgrind --leak-check=yes ./$(TARGET) --debug $(DAT_DIR)/HARVEST.DAT

clean:
	-@rm $(TARGET) 2>/dev/null || true
	-@rm $(TARGET)_i686.exe 2>/dev/null || true
	-@rm $(TARGET)_x86_64.exe 2>/dev/null || true
	-@rm -rf $(OUTPUT) 2>/dev/null || true

format: 
	clang-format -style=$(STYLE) -i $(SRC_FILES) $(INCL_FILES)

docs:
	doxygen docs/doxygen-config
