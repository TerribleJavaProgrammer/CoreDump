#include "board/position.h"
#include <iostream>
#include <sstream>

// Initializes all bitboards to the starting chess position
// Sets up the initial game state with pieces in their standard positions
void Position::initializePosition() {
    // Reset all bitboards to empty (all zeros)
    whitePawns = whiteKnights = whiteBishops = whiteRooks = whiteQueens = whiteKing = 0;
    blackPawns = blackKnights = blackBishops = blackRooks = blackQueens = blackKing = 0;
    occupiedSquares = whitePieces = blackPieces = emptySquares = 0;

    // Initialize white pieces
    for (int i = 8; i < 16; ++i) {
        whitePawns = setBit(whitePawns, i);  // Place pawns on 2nd rank
    }
    whiteRooks = setBit(whiteRooks, 0);      // A1
    whiteRooks = setBit(whiteRooks, 7);      // H1
    whiteKnights = setBit(whiteKnights, 1);  // B1
    whiteKnights = setBit(whiteKnights, 6);  // G1
    whiteBishops = setBit(whiteBishops, 2);  // C1
    whiteBishops = setBit(whiteBishops, 5);  // F1
    whiteQueens = setBit(whiteQueens, 3);    // D1
    whiteKing = setBit(whiteKing, 4);        // E1

    // Initialize black pieces
    for (int i = 48; i < 56; ++i) {
        blackPawns = setBit(blackPawns, i);  // Place pawns on 7th rank
    }
    blackRooks = setBit(blackRooks, 56);     // A8
    blackRooks = setBit(blackRooks, 63);     // H8
    blackKnights = setBit(blackKnights, 57); // B8
    blackKnights = setBit(blackKnights, 62); // G8
    blackBishops = setBit(blackBishops, 58); // C8
    blackBishops = setBit(blackBishops, 61); // F8
    blackQueens = setBit(blackQueens, 59);   // D8
    blackKing = setBit(blackKing, 60);       // E8

    // Update composite bitboards
    whitePieces = whitePawns | whiteKnights | whiteBishops | whiteRooks | whiteQueens | whiteKing;
    blackPieces = blackPawns | blackKnights | blackBishops | blackRooks | blackQueens | blackKing;
    occupiedSquares = whitePieces | blackPieces;
    emptySquares = ~occupiedSquares;         // All squares not occupied
}

// Computes the Zobrist hash of the position
uint64_t Position::computeHash() const {
    uint64_t hash = 0;

    auto hashPieces = [&](uint64_t bitboard, int pieceIndex) {
        while (bitboard) {
            int square = __builtin_ctzll(bitboard); // Get least significant bit index
            hash ^= zobristTable[pieceIndex][square];
            bitboard &= bitboard - 1; // Clear LSB
        }
    };

    hashPieces(whitePawns, 0);
    hashPieces(whiteKnights, 1);
    hashPieces(whiteBishops, 2);
    hashPieces(whiteRooks, 3);
    hashPieces(whiteQueens, 4);
    hashPieces(whiteKing, 5);
    hashPieces(blackPawns, 6);
    hashPieces(blackKnights, 7);
    hashPieces(blackBishops, 8);
    hashPieces(blackRooks, 9);
    hashPieces(blackQueens, 10);
    hashPieces(blackKing, 11);

    // En passant hash
    if (enPassantSquare != -1) {
        int file = enPassantSquare % 8;
        hash ^= zobristEnPassant[file];
    }

    // Castling rights hash
    hash ^= zobristCastling[castlingRights];

    return hash;
}

Position Position::getPosition() {
    Position newPos;
    newPos.whitePawns = whitePawns;
    newPos.whiteKnights = whiteKnights;
    newPos.whiteBishops = whiteBishops;
    newPos.whiteRooks = whiteRooks;
    newPos.whiteQueens = whiteQueens;
    newPos.whiteKing = whiteKing;
    
    newPos.blackPawns = blackPawns;
    newPos.blackKnights = blackKnights;
    newPos.blackBishops = blackBishops;
    newPos.blackRooks = blackRooks;
    newPos.blackQueens = blackQueens;
    newPos.blackKing = blackKing;
    
    newPos.whitePieces = whitePieces;
    newPos.blackPieces = blackPieces;
    newPos.occupiedSquares = occupiedSquares;
    newPos.emptySquares = emptySquares;
    
    newPos.castlingRights = castlingRights;
    newPos.enPassantSquare = enPassantSquare;
    
    return newPos;
}

// helper functions
// Displays the current chess board state in a human-readable format
// Uses Unicode chess pieces and coordinate system (a-h, 1-8)
std::string Position::displayPosition() {
    std::string returnable = "";
    // Loop through ranks from top (8) to bottom (1)
    for (int rank = 7; rank >= 0; --rank) {
        // Loop through files from left (a) to right (h)
        for (int file = 0; file < 8; ++file) {
            int square = rank * 8 + file;  // Convert rank/file to square index
            if (getBit(whitePawns, square)) returnable.append("♟ ");        // White pawn
            else if (getBit(whiteKnights, square)) returnable.append("♞ ");  // White knight
            else if (getBit(whiteBishops, square)) returnable.append("♝ ");  // White bishop
            else if (getBit(whiteRooks, square)) returnable.append("♜ ");    // White rook
            else if (getBit(whiteQueens, square)) returnable.append("♛ ");   // White queen
            else if (getBit(whiteKing, square)) returnable.append("♚ ");     // White king
            else if (getBit(blackPawns, square)) returnable.append("♙ ");    // Black pawn
            else if (getBit(blackKnights, square)) returnable.append("♘ ");  // Black knight
            else if (getBit(blackBishops, square)) returnable.append("♗ ");  // Black bishop
            else if (getBit(blackRooks, square)) returnable.append("♖ ");    // Black rook
            else if (getBit(blackQueens, square)) returnable.append("♕ ");   // Black queen
            else if (getBit(blackKing, square)) returnable.append("♔ ");     // Black king
            else returnable.append(". ");                                    // Empty square
        }
        returnable.append("/");
    }
    return returnable;
}

char Position::getSquareChar(int square) {
    if (getBit(whitePawns, square)) return 'P';    // white pawn
    if (getBit(whiteKnights, square)) return 'N';  // white knight
    if (getBit(whiteBishops, square)) return 'B';  // white bishop
    if (getBit(whiteRooks, square)) return 'R';    // white rook
    if (getBit(whiteQueens, square)) return 'Q';   // white queen
    if (getBit(whiteKing, square)) return 'K';     // white king
    if (getBit(blackPawns, square)) return 'p';    // black pawn
    if (getBit(blackKnights, square)) return 'n';  // black knight
    if (getBit(blackBishops, square)) return 'b';  // black bishop
    if (getBit(blackRooks, square)) return 'r';    // black rook
    if (getBit(blackQueens, square)) return 'q';   // black queen
    if (getBit(blackKing, square)) return 'k';     // black king
    return '.';                                    // empty square
}

std::string Position::getFen(bool whiteToMove, int halfmoveClock, int fullmoveNumber, std::string castlingRights, std::string enPassantTarget) {
    std::ostringstream fen;
    // Encode board position
    for (int rank = 7; rank >= 0; --rank) {
        int emptyCount = 0;
        for (int file = 0; file < 8; ++file) {
            int square = rank * 8 + file;
            char squareValue = getSquareChar(square);
            if (squareValue == '.') {
                ++emptyCount;
            } else {
                if (emptyCount > 0) {
                    fen << emptyCount;
                    emptyCount = 0;
                }
                fen << squareValue;
            }
        }
        if (emptyCount > 0) {
            fen << emptyCount;
        }
        if (rank > 0) {
            fen << '/';
        }
    }

    // Active color
    fen << ' ' << (whiteToMove ? 'w' : 'b');
    
    // Castling availability
    fen << ' ' << (castlingRights.empty() ? "-" : castlingRights);
    
    // En passant target
    fen << ' ' << (enPassantTarget.empty() ? "-" : enPassantTarget);
    
    // Halfmove clock and fullmove number
    fen << ' ' << halfmoveClock << ' ' << fullmoveNumber;
    
    return fen.str();
}

