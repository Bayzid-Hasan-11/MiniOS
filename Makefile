CC = gcc
CFLAGS = -Wall -Wextra -g -pthread -I./include
TARGET = minios
SRC = src/main.c src/shell/commands.c src/process/process_monitor.c src/memory/memory.c src/filesystem/file.c src/ipc/sync_demo.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

clean:
	rm -f $(TARGET)
