CC = gcc  # Compiler 
CFLAGS = -Wall -Wextra -pedantic -pthread # Compiler flags 
TARGET = assignment-3  # Name of the output executable

all: $(TARGET)

$(TARGET): assignment-3.o
	$(CC) $^ -o $@ $(CFLAGS)

assignment-3.o: assignment-3.c
	$(CC) -c $< -o $@ $(CFLAGS)

clean:
	rm -f *.o $(TARGET)
