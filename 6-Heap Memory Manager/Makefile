# Build type (DEBUG or RELEASE)
BUILD_TYPE = DEBUG

# Compiler and flags
CC = gcc

ifeq ($(BUILD_TYPE), DEBUG)
CFLAGS = -g -Wall -Wextra -I Level_1 -I Level_2 -I Level_3
else
CFLAGS = -O2 -Wall -Wextra -I Level_1 -I Level_2 -I Level_3
endif

# Target executable
TARGET = heap_manager

# Source files
SRCS = main.c \
       Level_1/HeapTest.c \
       Level_2/HeapExtras.c \
       Level_2/HeapManager.c \
       Level_3/HeapUtils.c

# Object files
OBJS = $(SRCS:.c=.o)

# Default target
all: $(TARGET)

# Compile each source file into an object file
main.o: main.c
	$(CC) $(CFLAGS) -o main.o -c main.c

Level_1/HeapTest.o: Level_1/HeapTest.c Level_1/HeapTest.h
	$(CC) $(CFLAGS) -o Level_1/HeapTest.o -c Level_1/HeapTest.c

Level_2/HeapExtras.o: Level_2/HeapExtras.c Level_2/HeapExtras.h
	$(CC) $(CFLAGS) -o Level_2/HeapExtras.o -c Level_2/HeapExtras.c

Level_2/HeapManager.o: Level_2/HeapManager.c Level_2/HeapManager.h
	$(CC) $(CFLAGS) -o Level_2/HeapManager.o -c Level_2/HeapManager.c

Level_3/HeapUtils.o: Level_3/HeapUtils.c Level_3/HeapUtils.h
	$(CC) $(CFLAGS) -o Level_3/HeapUtils.o -c Level_3/HeapUtils.c

# Link the object files to create the final executable
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

# Clean up object files and executable
clean:
	@rm -f $(OBJS)
	@rm -f $(TARGET)

.PHONY: clean

