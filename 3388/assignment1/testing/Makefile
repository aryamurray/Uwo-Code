CC = g++
CFLAGS = -c -Wall -g -Os -I./imgui -I./imgui/backends
LD = $(CC)
LDFLAGS = -lglfw -ldl -lm -lGL 

TARGET = main

SRCS = $(shell find . -name "*.cpp")
OBJECTS = $(SRCS:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(LD) -o $@ $^ $(LDFLAGS)

%.o: %.cpp
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f $(TARGET) $(OBJECTS)
