CC=mpic++
CFLAGS= -Wall -g
TARGET = parkmeans
SOURCES = $(wildcard *.cpp)

all: $(TARGET)

$(TARGET): $(SOURCES)
	$(CC) $^ -o $@ $(CFLAGS)

# args= numbers of processes
run: $(TARGET)
	mpirun -oversubscribe -np 5 ./$(TARGET)

clean:
	rm $(TARGET)