# Compiler and flags
CXX = g++
CXXFLAGS = -I./include
# Windows libraries (opengl32, glu32, freeglut)
LDFLAGS = -lopengl32 -lglu32 -lfreeglut

# Directories
SRC_DIR = src
INCLUDE_DIR = include

# Target executable
TARGET = game.exe

# Source files
# ADDED: $(SRC_DIR)/Fireball.cpp to the end
SOURCES = $(SRC_DIR)/main.cpp $(SRC_DIR)/Player.cpp $(SRC_DIR)/Mario.cpp $(SRC_DIR)/Luigi.cpp $(SRC_DIR)/Map.cpp $(SRC_DIR)/Platform.cpp $(SRC_DIR)/HUD.cpp $(SRC_DIR)/Menu.cpp $(SRC_DIR)/Fireball.cpp

# Default target
all: $(TARGET)

# Build the executable
$(TARGET): $(SOURCES)
	$(CXX) -o $(TARGET) $(CXXFLAGS) $(SOURCES) $(LDFLAGS)

# Run the application
run: $(TARGET)
	./$(TARGET)

# Clean build artifacts
clean:
	del $(TARGET)

# Rebuild everything
rebuild: clean all

.PHONY: all run clean rebuild