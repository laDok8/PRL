CC=mpic++
CFLAGS= -Wall -g
TARGET = parkmaens
SOURCES = $(wildcard *.cpp)

all: $(TARGET)

$(TARGET): $(SOURCES)
	$(CC) $^ -o $@ $(CFLAGS)

# args= numbers of processes
run: $(TARGET)
	mpirun -oversubscribe -np 10 ./$(TARGET)

clean:
	rm $(TARGET)