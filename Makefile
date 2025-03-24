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

# Object files (store them in "out" folder while preserving directory structure)
OBJ_FILES = $(patsubst chess_engine/src/%.cpp, $(OUT_DIR)/%.o, $(SRC_FILES))

# Default target - build the engine
all: $(TARGET)

# Build the chess engine
$(TARGET): $(OBJ_FILES) | $(OUT_DIR)
	@echo Linking: $@
	@$(CXX) $(CXXFLAGS) -o $@ $^

# Compile source files (place object files in "out")
$(OUT_DIR)/%.o: chess_engine/src/%.cpp | $(OUT_DIR)
	@echo Compiling: $<
	@mkdir -p $(dir $@)
	@$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Ensure output directory exists
$(OUT_DIR):
	@mkdir -p $(OUT_DIR)

# Clean build files
clean:
	@echo Cleaning...
	@rm -rf $(OUT_DIR)

run:
	@make clean
	@make all
	@./out/chess

# Phony targets
.PHONY: all clean
