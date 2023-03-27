CC=mpic++
CFLAGS= -Wall -g
TARGET = parsplit
SOURCES = $(wildcard *.cpp)

all: $(TARGET)

$(TARGET): $(SOURCES)
	$(CC) $^ -o $@ $(CFLAGS)

clean:
	rm $(TARGET)