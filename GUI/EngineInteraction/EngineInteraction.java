// This file interacts with the C++ code
package EngineInteraction;

public class EngineInteraction {
    static {
        System.loadLibrary("EngineInteraction");
        System.out.println("Library path: " + System.getProperty("java.library.path"));
    }

    public native boolean initGame(boolean isWhite); // initializes all that must be initialized; returns true when the game is initialized
    public native boolean makeMove(int fromSquare, int toSquare, boolean isWhite); // takes in and parses a move.
    // if the move is valid, true will be returned.
    public native String fetchState(boolean isWhite); // returns the game state with a string; this game state can then be parsed and displayed.
    public native boolean botMove(boolean botIsBlack); // prompts the AI to make a move; returns true when the bot has made its move.
    public native boolean isCheckMate(); // returns checkmate status
    public native boolean isStaleMate(); // returns stalemate status

    public static void main(String[] args) {
        System.out.println("Engine interaction started");
    }
}