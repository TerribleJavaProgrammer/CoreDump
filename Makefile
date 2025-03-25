# Compiler settings
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Ofast
INCLUDES = -Ichess_engine/include

# Output binary
TARGET = out/chess_engine_exe

# Output directory
OUT_DIR = out

# Source directories
SRC_DIR = chess_engine/src
BOARD_DIR = $(SRC_DIR)/board
ENGINE_DIR = $(SRC_DIR)/engine-related
HEURISTICS_DIR = $(SRC_DIR)/extraHeuristics
MOVE_DIR = $(SRC_DIR)/move

# Source files
MAIN_SRC = $(SRC_DIR)/main.cpp
BOARD_SRC = $(BOARD_DIR)/board.cpp $(BOARD_DIR)/magicbitboard.cpp $(BOARD_DIR)/position.cpp
ENGINE_SRC = $(ENGINE_DIR)/engine.cpp $(ENGINE_DIR)/evaluation.cpp $(ENGINE_DIR)/prioritization.cpp $(ENGINE_DIR)/search.cpp
HEURISTICS_SRC = $(HEURISTICS_DIR)/historyHeuristic.cpp $(HEURISTICS_DIR)/killerMoves.cpp $(HEURISTICS_DIR)/transposition.cpp $(HEURISTICS_DIR)/zobrist.cpp
MOVE_SRC = $(MOVE_DIR)/movegen.cpp $(MOVE_DIR)/nullMoveHandler.cpp

# All source files
SRC_FILES = $(MAIN_SRC) $(BOARD_SRC) $(ENGINE_SRC) $(HEURISTICS_SRC) $(MOVE_SRC)

# Object files
OBJ_FILES = $(patsubst $(SRC_DIR)/%.cpp,$(OUT_DIR)/%.o,$(SRC_FILES))

# JNI settings
JNI_HOME = C:/Users/aqeel/.jdks/openjdk-23.0.1
JNI_INCLUDES = -I$(JNI_HOME)/include -I$(JNI_HOME)/include/win32
JNI_SRC = GUI/EngineInteraction/EngineInteraction_EngineInteraction.cpp
JNI_TARGET = GUI/EngineInteraction/EngineInteraction.dll

# Java settings
JAVAC = C:/Users/aqeel/.jdks/openjdk-23.0.1/bin/javac.exe
JAVA = C:/Users/aqeel/.jdks/openjdk-23.0.1/bin/java.exe
JAVA_FILES = GUI/ChessGame.java GUI/EngineInteraction/EngineInteraction.java

# Ensure output directory exists
$(shell mkdir $(OUT_DIR) 2>nul)
$(shell mkdir $(OUT_DIR)\board 2>nul)
$(shell mkdir $(OUT_DIR)\engine-related 2>nul)
$(shell mkdir $(OUT_DIR)\extraHeuristics 2>nul)
$(shell mkdir $(OUT_DIR)\move 2>nul)

# Default target - build and run
all: $(TARGET) jni java run

# Build the chess engine
$(TARGET): $(OBJ_FILES)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Compile source files to object files
$(OUT_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# JNI library
jni:
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(JNI_INCLUDES) -shared -o $(JNI_TARGET) $(JNI_SRC) $(BOARD_SRC) $(ENGINE_SRC) $(HEURISTICS_SRC) $(MOVE_SRC)

# Compile Java files
java:
	$(JAVAC) $(JAVA_FILES)

# Run the application
run:
	@echo Running Chess GUI...
	@$(JAVA) -Djava.library.path=GUI/EngineInteraction -cp ".;GUI" ChessGame

# Clean build files
clean:
	if exist $(OUT_DIR) rmdir /S /Q $(OUT_DIR)
	del /Q GUI\EngineInteraction\*.class GUI\*.class
	del /Q GUI\EngineInteraction\EngineInteraction.dll

# Phony targets
.PHONY: all run clean jni java