#include "move/nullMove/nullMoveHandler.h"

std::vector<NullMoveState> nullMoveStack;

void makeNullMove(Position& pos) {
    // Save current state before making a null move
    nullMoveStack.push_back({pos.zobristKey, pos.enPassantSquare});
    
    // Null move: Toggle turn and clear en passant
    pos.zobristKey ^= ZOBRIST_TURN_KEY;
    pos.enPassantSquare = -1;
}

void undoNullMove(Position& pos) {
    // Restore previous state
    if (!nullMoveStack.empty()) {
        NullMoveState last = nullMoveStack.back();
        nullMoveStack.pop_back();
        pos.zobristKey = last.zobristKey;
        pos.enPassantSquare = last.enPassantSquare;
    }
}