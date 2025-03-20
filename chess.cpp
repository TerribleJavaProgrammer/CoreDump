/*
* Chess engine
* Created on: 3-6-25
*/

// Header files
#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <algorithm>
#include <cmath>
#include <memory>
#include <thread>
#include <future>
#include <mutex>
#include <chrono>
#include <array>
#include <random>
#include <bitset>
#include <unordered_map> 

// for magic bitboards
constexpr int BOARD_SIZE = 64;

// **BITBOARDS**
// white piece bitboards
uint64_t whitePawns;
uint64_t whiteKnights;
uint64_t whiteBishops;
uint64_t whiteRooks;
uint64_t whiteQueens;
uint64_t whiteKing;

// black piece bitboards
uint64_t blackPawns;
uint64_t blackKnights;
uint64_t blackBishops;
uint64_t blackRooks;
uint64_t blackQueens;
uint64_t blackKing;

// derivable bitboards
uint64_t occupiedSquares;
uint64_t whitePieces;
uint64_t blackPieces;
uint64_t emptySquares;

// **STRUCTS**
// Structure to represent a move
struct Move {
    int fromSquare;
    int toSquare;
    bool isCapture;
    enum class PieceType { PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING };
    PieceType pieceType;
    enum class Color { WHITE, BLACK };
    Color color;
};

// bitboard interaction function declarations
uint64_t setBit(uint64_t board, int square);
uint64_t clearBit(uint64_t board, int square);
bool getBit(uint64_t board, int square);
void initializeBitboards();
void updateBitboards(const Move& move);
void printBitBoard(uint64_t board);

// Shifts
std::array<int, 64> rookShifts;
std::array<int, 64> bishopShifts;

// Attack Tables
std::vector<uint64_t> rookAttackTable;
std::vector<uint64_t> bishopAttackTable;

int main() {
    initializeBitboards();

    // Basic game loop (alternating turns, manual move input)
    bool gameOver = false;
    Move currentMove;
    Move::Color currentPlayer = Move::Color::WHITE;

    while (!gameOver) {
        std::cout << "\nCurrent Board State (White Pieces Uppercase, Black Pieces Lowercase):\n";
        uint64_t allPieces = whitePieces | blackPieces;
        for (int rank = 7; rank >= 0; --rank) {
            for (int file = 0; file < 8; ++file) {
                int square = rank * 8 + file;
                if (getBit(whitePawns, square)) {
                    std::cout << "♙ ";
                } else if (getBit(whiteKnights, square)) {
                    std::cout << "♘ ";
                } else if (getBit(whiteBishops, square)) {
                    std::cout << "♗ ";
                } else if (getBit(whiteRooks, square)) {
                    std::cout << "♖ ";
                } else if (getBit(whiteQueens, square)) {
                    std::cout << "♕ ";
                } else if (getBit(whiteKing, square)) {
                    std::cout << "♔ ";
                } else if (getBit(blackPawns, square)) {
                    std::cout << "♟ ";
                } else if (getBit(blackKnights, square)) {
                    std::cout << "♞ ";
                } else if (getBit(blackBishops, square)) {
                    std::cout << "♝ ";
                } else if (getBit(blackRooks, square)) {
                    std::cout << "♜ ";
                } else if (getBit(blackQueens, square)) {
                    std::cout << "♛ ";
                } else if (getBit(blackKing, square)) {
                    std::cout << "♚ ";
                } else {
                    std::cout << ". ";
                }
            }
            std::cout << std::endl;
        }

        std::cout << (currentPlayer == Move::Color::WHITE ? "White" : "Black") << " to move.\n";

        // Get move input
        std::string moveStr;
        std::cout << "Enter move (e.g., a2 a3): ";
        std::getline(std::cin, moveStr);

        if (moveStr == "quit") {
            gameOver = true;
            continue;
        }

        if (moveStr.length() != 5 || moveStr[2] != ' ') {
            std::cout << "Invalid move format.\n";
            continue;
        }

        int fromFile = moveStr[0] - 'a';
        int fromRank = moveStr[1] - '1';
        int toFile = moveStr[3] - 'a';
        int toRank = moveStr[4] - '1';

        if (fromFile < 0 || fromFile > 7 || fromRank < 0 || fromRank > 7 ||
            toFile < 0 || toFile > 7 || toRank < 0 || toRank > 7) {
            std::cout << "Invalid square.\n";
            continue;
        }

        currentMove.fromSquare = fromRank * 8 + fromFile;
        currentMove.toSquare = toRank * 8 + toFile;
        if (currentMove.color == Move::Color::WHITE) {
            currentMove.isCapture = getBit(whitePieces, currentMove.toSquare); // Simple capture check
        } else {
            currentMove.isCapture = getBit(blackPieces, currentMove.toSquare); // Simple capture check
        }

        // Detect piece type
        if (getBit(whitePawns, currentMove.fromSquare)) {
            currentMove.pieceType = Move::PieceType::PAWN;
        } else if (getBit(whiteKnights, currentMove.fromSquare)) {
            currentMove.pieceType = Move::PieceType::KNIGHT;
        } else if (getBit(whiteBishops, currentMove.fromSquare)) {
            currentMove.pieceType = Move::PieceType::BISHOP;
        } else if (getBit(whiteRooks, currentMove.fromSquare)) {
            currentMove.pieceType = Move::PieceType::ROOK;
        } else if (getBit(whiteQueens, currentMove.fromSquare)) {
            currentMove.pieceType = Move::PieceType::QUEEN;
        } else if (getBit(whiteKing, currentMove.fromSquare)) {
            currentMove.pieceType = Move::PieceType::KING;
        } else if (getBit(blackPawns, currentMove.fromSquare)) {
            currentMove.pieceType = Move::PieceType::PAWN;
        } else if (getBit(blackKnights, currentMove.fromSquare)) {
            currentMove.pieceType = Move::PieceType::KNIGHT;
        } else if (getBit(blackBishops, currentMove.fromSquare)) {
            currentMove.pieceType = Move::PieceType::BISHOP;
        } else if (getBit(blackRooks, currentMove.fromSquare)) {
            currentMove.pieceType = Move::PieceType::ROOK;
        } else if (getBit(blackQueens, currentMove.fromSquare)) {
            currentMove.pieceType = Move::PieceType::QUEEN;
        } else if (getBit(blackKing, currentMove.fromSquare)) {
            currentMove.pieceType = Move::PieceType::KING;
        }

        currentMove.color = currentPlayer;

        // Make the move
        updateBitboards(currentMove);

        // Switch player
        currentPlayer = (currentPlayer == Move::Color::WHITE) ? Move::Color::BLACK : Move::Color::WHITE;
    }

    std::cout << "Game Over.\n";

    return 0;
}

// **BITBOARD INTERACTION FUNCTIONS**
// bit setter
uint64_t setBit(uint64_t board, int square) {
    return board | (1ULL << square);
}

// bit clearer
uint64_t clearBit(uint64_t board, int square) {
    return board & ~(1ULL << square);
}

// bit getter
bool getBit(uint64_t board, int square) {
    return (board >> square) & 1;
}

// Function to initialize the bitboards to the starting position
void initializeBitboards() {
    // Reset all bitboards
    whitePawns = 0;
    whiteKnights = 0;
    whiteBishops = 0;
    whiteRooks = 0;
    whiteQueens = 0;
    whiteKing = 0;
    blackPawns = 0;
    blackKnights = 0;
    blackBishops = 0;
    blackRooks = 0;
    blackQueens = 0;
    blackKing = 0;
    occupiedSquares = 0;
    whitePieces = 0;
    blackPieces = 0;
    emptySquares = 0;

    // Initialize white pieces (using rank-major square indexing)
    for (int i = 8; i < 16; ++i) whitePawns = setBit(whitePawns, i); // Pawns on rank 2
    whiteRooks = setBit(whiteRooks, 0);   // A1
    whiteRooks = setBit(whiteRooks, 7);   // H1
    whiteKnights = setBit(whiteKnights, 1); // B1
    whiteKnights = setBit(whiteKnights, 6); // G1
    whiteBishops = setBit(whiteBishops, 2); // C1
    whiteBishops = setBit(whiteBishops, 5); // F1
    whiteQueens = setBit(whiteQueens, 3);  // D1
    whiteKing = setBit(whiteKing, 4);      // E1

    // Initialize black pieces
    for (int i = 48; i < 56; ++i) blackPawns = setBit(blackPawns, i); // Pawns on rank 7
    blackRooks = setBit(blackRooks, 56);   // A8
    blackRooks = setBit(blackRooks, 63);   // H8
    blackKnights = setBit(blackKnights, 57); // B8
    blackKnights = setBit(blackKnights, 62); // G8
    blackBishops = setBit(blackBishops, 58); // C8
    blackBishops = setBit(blackBishops, 61); // F8
    blackQueens = setBit(blackQueens, 59); // D8
    blackKing = setBit(blackKing, 60);    // E8

    // Update derivable bitboards
    whitePieces = whitePawns | whiteKnights | whiteBishops | whiteRooks | whiteQueens | whiteKing;
    blackPieces = blackPawns | blackKnights | blackBishops | blackRooks | blackQueens | blackKing;
    occupiedSquares = whitePieces | blackPieces;
    emptySquares = ~occupiedSquares;
}

// Function to update the bitboards based on a move
void updateBitboards(const Move& move) {
    // Clear the piece from its original square
    if (move.color == Move::Color::WHITE) {
        switch (move.pieceType) {
            case Move::PieceType::PAWN: whitePawns = clearBit(whitePawns, move.fromSquare); break;
            case Move::PieceType::KNIGHT: whiteKnights = clearBit(whiteKnights, move.fromSquare); break;
            case Move::PieceType::BISHOP: whiteBishops = clearBit(whiteBishops, move.fromSquare); break;
            case Move::PieceType::ROOK: whiteRooks = clearBit(whiteRooks, move.fromSquare); break;
            case Move::PieceType::QUEEN: whiteQueens = clearBit(whiteQueens, move.fromSquare); break;
            case Move::PieceType::KING: whiteKing = clearBit(whiteKing, move.fromSquare); break;
        }
    } else {
        switch (move.pieceType) {
            case Move::PieceType::PAWN: blackPawns = clearBit(blackPawns, move.fromSquare); break;
            case Move::PieceType::KNIGHT: blackKnights = clearBit(blackKnights, move.fromSquare); break;
            case Move::PieceType::BISHOP: blackBishops = clearBit(blackBishops, move.fromSquare); break;
            case Move::PieceType::ROOK: blackRooks = clearBit(blackRooks, move.fromSquare); break;
            case Move::PieceType::QUEEN: blackQueens = clearBit(blackQueens, move.fromSquare); break;
            case Move::PieceType::KING: blackKing = clearBit(blackKing, move.fromSquare); break;
        }
    }

    // Handle captures if any
    if (move.color == Move::Color::WHITE && (blackPieces & (1ULL << move.toSquare))) {
        if (getBit(blackPawns, move.toSquare)) blackPawns = clearBit(blackPawns, move.toSquare);
        if (getBit(blackKnights, move.toSquare)) blackKnights = clearBit(blackKnights, move.toSquare);
        if (getBit(blackBishops, move.toSquare)) blackBishops = clearBit(blackBishops, move.toSquare);
        if (getBit(blackRooks, move.toSquare)) blackRooks = clearBit(blackRooks, move.toSquare);
        if (getBit(blackQueens, move.toSquare)) blackQueens = clearBit(blackQueens, move.toSquare);
    } else if (move.color == Move::Color::BLACK && (whitePieces & (1ULL << move.toSquare))) {
        if (getBit(whitePawns, move.toSquare)) whitePawns = clearBit(whitePawns, move.toSquare);
        if (getBit(whiteKnights, move.toSquare)) whiteKnights = clearBit(whiteKnights, move.toSquare);
        if (getBit(whiteBishops, move.toSquare)) whiteBishops = clearBit(whiteBishops, move.toSquare);
        if (getBit(whiteRooks, move.toSquare)) whiteRooks = clearBit(whiteRooks, move.toSquare);
        if (getBit(whiteQueens, move.toSquare)) whiteQueens = clearBit(whiteQueens, move.toSquare);
    }

    // Place the piece on its new square
    if (move.color == Move::Color::WHITE) {
        switch (move.pieceType) {
            case Move::PieceType::PAWN: whitePawns = setBit(whitePawns, move.toSquare); break;
            case Move::PieceType::KNIGHT: whiteKnights = setBit(whiteKnights, move.toSquare); break;
            case Move::PieceType::BISHOP: whiteBishops = setBit(whiteBishops, move.toSquare); break;
            case Move::PieceType::ROOK: whiteRooks = setBit(whiteRooks, move.toSquare); break;
            case Move::PieceType::QUEEN: whiteQueens = setBit(whiteQueens, move.toSquare); break;
            case Move::PieceType::KING: whiteKing = setBit(whiteKing, move.toSquare); break;
        }
    } else {
        switch (move.pieceType) {
            case Move::PieceType::PAWN: blackPawns = setBit(blackPawns, move.toSquare); break;
            case Move::PieceType::KNIGHT: blackKnights = setBit(blackKnights, move.toSquare); break;
            case Move::PieceType::BISHOP: blackBishops = setBit(blackBishops, move.toSquare); break;
            case Move::PieceType::ROOK: blackRooks = setBit(blackRooks, move.toSquare); break;
            case Move::PieceType::QUEEN: blackQueens = setBit(blackQueens, move.toSquare); break;
            case Move::PieceType::KING: blackKing = setBit(blackKing, move.toSquare); break;
        }
    }

    // Update composite bitboards
    whitePieces = whitePawns | whiteKnights | whiteBishops | whiteRooks | whiteQueens | whiteKing;
    blackPieces = blackPawns | blackKnights | blackBishops | blackRooks | blackQueens | blackKing;
    occupiedSquares = whitePieces | blackPieces;
    emptySquares = ~occupiedSquares;
}

// debugging function
void printBitBoard(uint64_t board) {
    for (int rank = 7; rank >= 0; --rank) {
        for (int file = 0; file < 8; ++file) {
            int square = rank * 8 + file;
            std::cout << (getBit(board, square) ? "1 " : "0 ");
        }
        std::cout << std::endl;
    }
}

// end of file