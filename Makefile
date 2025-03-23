CXX := g++
CXXFLAGS := -std=c++20 -O2 -Wall -Wextra -Iinclude -Ichess_engine/include
SRC_DIR := chess_engine/src
OBJ_DIR := chess_engine/obj
BIN := chess-engine

# Manually define subdirectories
SRC_SUBDIRS := board engine-related extraHeuristics move
SOURCES := $(wildcard $(SRC_DIR)/*.cpp) \
           $(foreach dir, $(SRC_SUBDIRS), $(wildcard $(SRC_DIR)/$(dir)/*.cpp))

OBJECTS := $(SOURCES:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

# Default target
all: $(BIN)

# Link final executable
$(BIN): $(OBJECTS)
	$(CXX) $(CXXFLAGS) $^ -o $@

# Compile each .cpp into .o, ensuring directories exist
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@if not exist "$(dir $@)" mkdir "$(dir $@)"
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean compiled files
clean:
	del /Q /F $(BIN) $(OBJ_DIR)\*.o

.PHONY: all clean