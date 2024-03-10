# --- Compiler and flags ---
CC = g++
CFLAGS = -Wall -g 

# --- Directories ---
SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin

# --- Discover C source files ---
SRCS := $(wildcard $(SRC_DIR)/*.cpp)

# --- Create object file names ---
OBJS := $(SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

# --- Name of your final executable ---
TARGET = assignment4

# --- Libraries ---
LIBS = -lglfw -ldl -lm -lGL 

# --- All target: the default when you run 'make' ---
all: $(BIN_DIR)/$(TARGET)

# --- Create output directories if they don't exist ---
$(OBJ_DIR) $(BIN_DIR):
	mkdir -p $@

# --- Discover header files ---
HDRS := $(wildcard $(SRC_DIR)/*.hpp)

# --- Rule to compile an object file from a C source file---
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp $(HDRS)
	$(CC) $(CFLAGS) -c $< -o $@

# --- Rule to link the final executable ---
$(BIN_DIR)/$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS)

# --- Useful 'clean' target to remove build artifacts---
clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)
