CC=mpic++
CFLAGS= -Wall -g
TARGET = parsplit
SOURCES = $(wildcard *.cpp)

all: $(TARGET)

$(TARGET): $(SOURCES)
	$(CC) $^ -o $@ $(CFLAGS)

run: $(TARGET)
	mpirun -np 6 ./$(TARGET)

clean:
	rm $(TARGET)