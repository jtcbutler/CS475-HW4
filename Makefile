CC=gcc
CFLAGS=-Wall -Iinclude -g
BUILD_DIR=.
SOURCE_DIR=src
TARGET=dsh

default: main.o dsh.o valid_malloc.o dynamic_string.o pid_list.o
	$(CC) $(CFLAGS) $(BUILD_DIR)/*.o -o $(BUILD_DIR)/$(TARGET)
	rm $(BUILD_DIR)/*.o

main.o:
	$(CC) $(CFLAGS) -c $(SOURCE_DIR)/main.c -o $(BUILD_DIR)/main.o

dsh.o:
	$(CC) $(CFLAGS) -c $(SOURCE_DIR)/dsh.c -o $(BUILD_DIR)/dsh.o

valid_malloc.o:
	$(CC) $(CFLAGS) -c $(SOURCE_DIR)/valid_malloc.c -o $(BUILD_DIR)/valid_malloc.o

dynamic_string.o:
	$(CC) $(CFLAGS) -c $(SOURCE_DIR)/dynamic_string.c -o $(BUILD_DIR)/dynamic_string.o

pid_list.o:
	$(CC) $(CFLAGS) -c $(SOURCE_DIR)/pid_list.c -o $(BUILD_DIR)/pid_list.o

run: default
	$(BUILD_DIR)/$(TARGET)
