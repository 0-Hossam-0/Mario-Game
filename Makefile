# Compiler and flags
CXX = g++
CXXFLAGS = -I./include
LDFLAGS = -lGL -lGLU -lglut -lm

# Directories
SRC_DIR = src
INCLUDE_DIR = include

# Target executable
TARGET = game

# Source files
# Source files
SOURCES = $(SRC_DIR)/main.cpp $(SRC_DIR)/Player.cpp $(SRC_DIR)/Mario.cpp $(SRC_DIR)/Luigi.cpp $(SRC_DIR)/Map.cpp $(SRC_DIR)/Platform.cpp $(SRC_DIR)/HUD.cpp $(SRC_DIR)/Menu.cpp $(SRC_DIR)/FireBall.cpp $(SRC_DIR)/Bomb.cpp $(SRC_DIR)/Star.cpp

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
	rm -f $(TARGET)

# Rebuild everything
rebuild: clean all

.PHONY: all run clean rebuild
