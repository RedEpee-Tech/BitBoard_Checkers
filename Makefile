# Simple Makefile for BitBoard_Checkers

CC := gcc
CFLAGS := -std=c11 -Wall -Wextra -O2

SRCS := BitBoard_Checkers.c
OBJS := $(SRCS:.c=.o)

# Output executable name (use .exe on Windows)
ifeq ($(OS),Windows_NT)
	OUT := checkers.exe
else
	OUT := checkers
endif

.PHONY: all clean run

all: $(OUT)

$(OUT): $(SRCS)
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f $(OUT) $(OBJS)

run: $(OUT)
	./$(OUT)
