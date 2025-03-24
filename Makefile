# Makefile for building and running C++ with JNI

# Set the path to the JDK (adjust if necessary, try to rely on environment)
JAVAC = javac
JAVA = java
# If needed, you can try to find JAVA_HOME based on environment, but relying on the user’s environment should be sufficient
JAVA_HOME ?= $(shell dirname $(shell dirname $(shell which java)))
JNI_INCLUDE_PATH = $(JAVA_HOME)/include
JNI_INCLUDE_PATH_WIN = $(JAVA_HOME)/include/win32

# Path to the 64-bit g++ compiler (adjust this to your actual path)
MINGW64_GPP = /mingw64/bin/x86_64-w64-mingw32-g++.exe
CC = $(MINGW64_GPP)
CXXFLAGS = -std=c++11 -I"./chess_engine/include" -I"$(JNI_INCLUDE_PATH)" -I"$(JNI_INCLUDE_PATH_WIN)"
LDFLAGS = -shared -o ./GUI/engine.dll

# File locations
SRC_DIR = ./chess_engine/src
OBJ_DIR = ./chess_engine/obj
JNI_DIR = ./GUI/EngineInteraction/EngineInteraction

# List of source files
SRC_FILES = $(SRC_DIR)/perft.cpp \
            $(SRC_DIR)/board/board.cpp \
            $(SRC_DIR)/board/magicbitboard.cpp \
            $(SRC_DIR)/board/position.cpp \
            $(SRC_DIR)/engine-related/engine.cpp \
            $(SRC_DIR)/engine-related/evaluation.cpp \
            $(SRC_DIR)/engine-related/prioritization.cpp \
            $(SRC_DIR)/engine-related/search.cpp \
            $(SRC_DIR)/extraHeuristics/historyHeuristic.cpp \
            $(SRC_DIR)/extraHeuristics/killerMoves.cpp \
            $(SRC_DIR)/extraHeuristics/transposition.cpp \
            $(SRC_DIR)/extraHeuristics/zobrist.cpp \
            $(SRC_DIR)/move/movegen.cpp \
            $(SRC_DIR)/move/nullMoveHandler.cpp

# List of object files (automatically generated)
OBJ_FILES = $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRC_FILES))

# Create object directory if it doesn't exist
$(OBJ_DIR):
	@mkdir -p $@

# Default target to compile and run
all: compile_library run_java

# Compile C++ source files into object files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	$(CC) $(CXXFLAGS) -c $< -o $@

# Create the shared library from object files
compile_library: $(OBJ_FILES)
	$(CC) $(CXXFLAGS) $(OBJ_FILES) $(LDFLAGS)

# Compile and run Java code
run_java:
	$(JAVAC) ./GUI/EngineInteraction/EngineInteraction.java ./GUI/ChessGame.java
	$(JAVA) -Djava.library.path=./GUI -cp ./GUI ChessGame

# Clean up compiled files
clean:
	rm -f $(OBJ_FILES) ./GUI/engine.dll
	rm -f ./GUI/*.class