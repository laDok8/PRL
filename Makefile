C=g++
CFLAGS= -Wall -g
TARGET = parsplit
SOURCES = $(wildcard *.c)

all: $(TARGET)

$(TARGET): $(SOURCES)
	$(CC) $^ -o $@ $(CFLAGS)

clean:
	rm $(TARGET)