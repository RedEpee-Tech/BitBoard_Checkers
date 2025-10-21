# # Simple Makefile for BitBoard_Checkers

# CC := gcc
# CFLAGS := -std=c11 -Wall -Wextra -O2

# SRCS := BitBoard_Checkers.c
# OBJS := $(SRCS:.c=.o)

# # Output executable name (use .exe on Windows)
# ifeq ($(OS),Windows_NT)
# 	OUT := checkers.exe
# else
# 	OUT := checkers
# endif

# .PHONY: all clean run

# all: $(OUT)

# $(OUT): $(SRCS)
# 	$(CC) $(CFLAGS) -o $@ $^

# clean:
# 	rm -f $(OUT) $(OBJS)

# run: $(OUT)
# 	./$(OUT)

# ========================================
# Makefile for BitBoard Checkers (C)
# ========================================

# Compiler and flags
CC      := gcc
CFLAGS  := -Wall -Wextra -std=c11
DEBUG   := -g
TARGET  := checkers
SRC     := BitBoard_Checkers.c
OBJ     := $(SRC:.c=.o)

# Default build rule
all: $(TARGET)

# Build the main executable
$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

# Compile object file
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Run the game
run: $(TARGET)
	./$(TARGET)

# Debug build
debug: CFLAGS += $(DEBUG)
debug: clean $(TARGET)
	@echo "Built with debug flags."

# Clean up build files
clean:
	rm -f $(OBJ) $(TARGET)

# Save current game state (optional helper)
save:
	@echo "Saving current game state to checkers_save.dat..."
	@./$(TARGET) save checkers_save.dat

# Load saved game (optional helper)
load:
	@echo "Loading saved game from checkers_save.dat..."
	@./$(TARGET) load checkers_save.dat



# Help menu
help:
	@echo "Available commands:"
	@echo "  make          - Build the game"
	@echo "  make run      - Run the game"
	@echo "  make debug    - Build with debug symbols"
	@echo "  make clean    - Remove binaries and object files"
	@echo "  make save     - Save current game (optional)"
	@echo "  make load     - Load previous game (optional)"
