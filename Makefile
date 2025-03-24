# Compiler settings
CXX = g++ # or clang++
CXXFLAGS = -Ofast -march=native -flto -Wall -Wextra
INCLUDES = -Ichess_engine/include

# Output directory
OUT_DIR = out

# Output binary
TARGET = $(OUT_DIR)/chess

# Source directories
SRC_DIR = chess_engine/src
BOARD_DIR = $(SRC_DIR)/board
ENGINE_DIR = $(SRC_DIR)/engine-related
HEURISTICS_DIR = $(SRC_DIR)/extraHeuristics
MOVE_DIR = $(SRC_DIR)/move

# Source files
MAIN_SRC = $(SRC_DIR)/main.cpp $(SRC_DIR)/perft.cpp
BOARD_SRC = $(BOARD_DIR)/board.cpp $(BOARD_DIR)/magicbitboard.cpp $(BOARD_DIR)/position.cpp
ENGINE_SRC = $(ENGINE_DIR)/engine.cpp $(ENGINE_DIR)/evaluation.cpp $(ENGINE_DIR)/prioritization.cpp $(ENGINE_DIR)/search.cpp
HEURISTICS_SRC = $(HEURISTICS_DIR)/historyHeuristic.cpp $(HEURISTICS_DIR)/killerMoves.cpp $(HEURISTICS_DIR)/transposition.cpp $(HEURISTICS_DIR)/zobrist.cpp
MOVE_SRC = $(MOVE_DIR)/movegen.cpp $(MOVE_DIR)/nullMoveHandler.cpp

# All source files
SRC_FILES = $(MAIN_SRC) $(BOARD_SRC) $(ENGINE_SRC) $(HEURISTICS_SRC) $(MOVE_SRC)

# Object files (store them in "out" folder)
OBJ_FILES = $(patsubst %.cpp, $(OUT_DIR)/%.o, $(SRC_FILES))

# Ensure output directory exists (Windows & Unix compatible)
$(shell if not exist $(OUT_DIR) mkdir $(OUT_DIR))

# Default target - build the engine
all: $(OUT_DIR) $(TARGET)

# Build the chess engine
$(TARGET): $(OBJ_FILES)
	@$(CXX) $(CXXFLAGS) -o $@ $^

# Compile source files (place object files in "out")
$(OUT_DIR)/%.o: %.cpp | $(OUT_DIR)
	@echo Compiling: $<
	@if not exist "$(dir $@)" mkdir "$(dir $@)"
	@$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Create output directory if not existing (Windows-friendly)
$(OUT_DIR):
	@if not exist $(OUT_DIR) mkdir $(OUT_DIR)

# Clean build files (Windows & Unix compatible)
clean:
	@echo Cleaning... $<
	@if exist $(OUT_DIR) rmdir /s /q $(OUT_DIR) & if exist $(OUT_DIR) rm -rf $(OUT_DIR)

# Phony targets
.PHONY: all clean