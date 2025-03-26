# Compiler settings
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Ofast
INCLUDES = -Ichess_engine/include

# Output binary
TARGET = out/chess

# Output directory
OUT_DIR = out

# Source directories
SRC_DIR = chess_engine/src
BOARD_DIR = $(SRC_DIR)/board
ENGINE_DIR = $(SRC_DIR)/engine-related
HEURISTICS_DIR = $(SRC_DIR)/extraHeuristics
MOVE_DIR = $(SRC_DIR)/move

# Source files
MAIN_SRC = GUI/API/API.cpp
BOARD_SRC = $(BOARD_DIR)/board.cpp $(BOARD_DIR)/magicbitboard.cpp $(BOARD_DIR)/position.cpp
ENGINE_SRC = $(ENGINE_DIR)/engine.cpp $(ENGINE_DIR)/evaluation.cpp $(ENGINE_DIR)/prioritization.cpp $(ENGINE_DIR)/search.cpp
HEURISTICS_SRC = $(HEURISTICS_DIR)/historyHeuristic.cpp $(HEURISTICS_DIR)/killerMoves.cpp $(HEURISTICS_DIR)/transposition.cpp $(HEURISTICS_DIR)/zobrist.cpp
MOVE_SRC = $(MOVE_DIR)/movegen.cpp $(MOVE_DIR)/nullMoveHandler.cpp

# Object files for source files in chess_engine/src
SRC_OBJ_FILES = $(patsubst $(SRC_DIR)/%.cpp,$(OUT_DIR)/%.o,$(BOARD_SRC) $(ENGINE_SRC) $(HEURISTICS_SRC) $(MOVE_SRC))

# Object files for main source file
MAIN_OBJ_FILE = $(OUT_DIR)/API.o

# All object files
OBJ_FILES = $(MAIN_OBJ_FILE) $(SRC_OBJ_FILES)

# Ensure output directory exists
$(shell mkdir $(OUT_DIR) 2>nul)
$(shell mkdir $(OUT_DIR)\board 2>nul)
$(shell mkdir $(OUT_DIR)\engine-related 2>nul)
$(shell mkdir $(OUT_DIR)\extraHeuristics 2>nul)
$(shell mkdir $(OUT_DIR)\move 2>nul)

# Default target - build and run the entire project (GUI alongside API)
all: build_api run_gui

# Task 1: Build API.cpp (compile and link API source file into an executable)
build_api: $(TARGET)
	@echo "API compiled and linked."

# Build the chess engine
$(TARGET): $(OBJ_FILES)
	@$(CXX) $(CXXFLAGS) -o $@ $^

# Compile GUI API source file
$(OUT_DIR)/API.o: GUI/API/API.cpp
	@$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Compile source files to object files
$(OUT_DIR)/%.o: $(SRC_DIR)/%.cpp
	@$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Task 2: Run the GUI after building the API
run_gui: build_api
	@echo "Running Chess GUI..."
	@python GUI/gui.py

# Clean build files
clean:
	@if exist $(OUT_DIR) rmdir /S /Q $(OUT_DIR)
	@del GUI\API\__pycache__\*.pyc

# Phony targets
.PHONY: all build_api run_gui clean
