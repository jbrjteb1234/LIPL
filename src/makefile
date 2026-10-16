# Compiler and flags
CC = gcc
CFLAGS = -std=c11 -g -Wall -Wextra -pedantic -Wmissing-prototypes -Wstrict-prototypes -Wshadow
# -MMD -MP: also write .d files so editing a header rebuilds everything that includes it
DEPFLAGS = -MMD -MP
# the interpreter uses pow, sqrt, cbrt and fmod
LDLIBS = -lm

# Windows gives executables a 2 MB stack; ask for the 8 MB Linux has, so recursion goes as deep
ifeq ($(OS),Windows_NT)
LDLIBS += -Wl,--stack,8388608
endif

# `make SANITIZE=1` builds with AddressSanitizer and UBSan (Linux/WSL; MinGW has no ASan)
ifdef SANITIZE
CFLAGS += -fsanitize=address,undefined -fno-omit-frame-pointer
endif

# Directories
SRC_DIR = .
OBJ_DIR = ./obj

# Find all .c files recursively and corresponding .o files
SRC_FILES = $(shell find $(SRC_DIR) -type f -name '*.c' -not -path '$(OBJ_DIR)/*')
OBJ_FILES = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRC_FILES))

# Output executable
TARGET = lipl

# Default rule: build the target
all: $(TARGET)

# Link .o files into the final executable
$(TARGET): $(OBJ_FILES)
	$(CC) $(CFLAGS) -o $@ $^ $(LDLIBS)

# Rule for compiling .c files into .o files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $(DEPFLAGS) -c $< -o $@

# Run the test suite in ../tests
test: $(TARGET)
	bash ../tests/run_tests.sh ./$(TARGET)

# Clean up build artifacts
clean:
	rm -rf $(OBJ_DIR) $(TARGET) $(TARGET).exe

-include $(OBJ_FILES:.o=.d)

# Phony targets
.PHONY: all clean test
