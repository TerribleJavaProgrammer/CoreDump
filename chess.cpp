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


// Initialize magic lookup tables
std::array<MagicEntry, 64> rookTable;
std::array<MagicEntry, 64> bishopTable;

// pre-calculated magic numbers
constexpr std::array<uint64_t, 64> ROOK_MAGICS = {
    0x0080001020400080ULL, 0x0040001000200040ULL, 0x0080081000200080ULL, 0x0080040800100080ULL,
    0x0080020400080080ULL, 0x0080010200040080ULL, 0x0080008001000200ULL, 0x0080002040800100ULL,
    0x0000800020400080ULL, 0x0000400020005000ULL, 0x0000801000200080ULL, 0x0000800800100080ULL,
    0x0000800400080080ULL, 0x0000800200040080ULL, 0x0000800100020080ULL, 0x0000800040800100ULL,
    0x0000208000400080ULL, 0x0000404000201000ULL, 0x0000808010002000ULL, 0x0000808008001000ULL,
    0x0000808004000800ULL, 0x0000808002000400ULL, 0x0000010100020004ULL, 0x0000020000408104ULL,
    0x0000208080004000ULL, 0x0000200040005000ULL, 0x0000100080200080ULL, 0x0000080080100080ULL,
    0x0000040080080080ULL, 0x0000020080040080ULL, 0x0000010080800200ULL, 0x0000800080004100ULL,
    0x0000204000800080ULL, 0x0000200040401000ULL, 0x0000100080802000ULL, 0x0000080080801000ULL,
    0x0000040080800800ULL, 0x0000020080800400ULL, 0x0000020001010004ULL, 0x0000800040800100ULL,
    0x0000204000808000ULL, 0x0000200040008080ULL, 0x0000100020008080ULL, 0x0000080010008080ULL,
    0x0000040008008080ULL, 0x0000020004008080ULL, 0x0000010002008080ULL, 0x0000004081020004ULL,
    0x0000204000800080ULL, 0x0000200040008080ULL, 0x0000100020008080ULL, 0x0000080010008080ULL,
    0x0000040008008080ULL, 0x0000020004008080ULL, 0x0000800100020080ULL, 0x0000800041000080ULL,
    0x00FFFCDDFCED714AULL, 0x007FFCDDFCED714AULL, 0x003FFFCDFFD88096ULL, 0x0000040810002101ULL,
    0x0001000204080011ULL, 0x0001000204000801ULL, 0x0001000082000401ULL, 0x0001FFFAABFAD1A2ULL
};

constexpr std::array<uint64_t, 64> BISHOP_MAGICS = {
    0x0002020202020200ULL, 0x0002020202020000ULL, 0x0004010202000000ULL, 0x0004040080000000ULL,
    0x0001104000000000ULL, 0x0000821040000000ULL, 0x0000410410400000ULL, 0x0000104104104000ULL,
    0x0000040404040400ULL, 0x0000020202020200ULL, 0x0000040102020000ULL, 0x0000040400800000ULL,
    0x0000011040000000ULL, 0x0000008210400000ULL, 0x0000004104104000ULL, 0x0000002082082000ULL,
    0x0004000808080800ULL, 0x0002000404040400ULL, 0x0001000202020200ULL, 0x0000800802004000ULL,
    0x0000800400A00000ULL, 0x0000200100884000ULL, 0x0000400082082000ULL, 0x0000200041041000ULL,
    0x0002080010101000ULL, 0x0001040008080800ULL, 0x0000208004010400ULL, 0x0000404004010200ULL,
    0x0000840000802000ULL, 0x0000404002011000ULL, 0x0000808001041000ULL, 0x0000404000820800ULL,
    0x0001041000202000ULL, 0x0000820800101000ULL, 0x0000104400080800ULL, 0x0000020080080080ULL,
    0x0000404040040100ULL, 0x0000808100020100ULL, 0x0001010100020800ULL, 0x0000808080010400ULL,
    0x0000820820004000ULL, 0x0000410410002000ULL, 0x0000082088001000ULL, 0x0000002011000800ULL,
    0x0000080100400400ULL, 0x0001010101000200ULL, 0x0002020202000400ULL, 0x0001010101000200ULL,
    0x0000410410400000ULL, 0x0000208208200000ULL, 0x0000002084100000ULL, 0x0000000020880000ULL,
    0x0000001002020000ULL, 0x0000040408020000ULL, 0x0004040404040000ULL, 0x0002020202020000ULL,
    0x0000104104104000ULL, 0x0000002082082000ULL, 0x0000000020841000ULL, 0x0000000000208800ULL,
    0x0000000010020200ULL, 0x0000000404080200ULL, 0x0000040404040400ULL, 0x0002020202020200ULL
};

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

CastlingRights castlingRights;

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
    bool isCastling;
    enum class CastlingType { NONE, KINGSIDE, QUEENSIDE };
    CastlingType castlingType;
    bool isPromotion;
    PieceType promotionPiece;
};
struct MagicEntry {
    uint64_t mask;
    uint64_t magic;
    int shift;
    std::vector<uint64_t> attacks;
};
struct CastlingRights {
    bool whiteKingside = true;
    bool whiteQueenside = true;
    bool blackKingside = true;
    bool blackQueenside = true;
};

// function declarations
uint64_t setBit(uint64_t board, int square);
uint64_t clearBit(uint64_t board, int square);
bool getBit(uint64_t board, int square);
void initializeBitboards();
void updateBitboards(const Move& move);
void printBitBoard(uint64_t board);
void displayBoard();
void displayLegalMoves(int square, Move::Color color);
uint64_t generateRookMask(int square);
uint64_t generateBishopMask(int square);
uint64_t generateRookAttacks(int square, uint64_t blockers);
uint64_t generateBishopAttacks(int square, uint64_t blockers);
void initializeMagicBitboards();
uint64_t getRookMoves(int square, uint64_t occupied);
uint64_t getBishopMoves(int square, uint64_t occupied);
uint64_t getPawnMoves(int square, Move::Color color, uint64_t occupied);
uint64_t getKnightMoves(int square);
uint64_t getKingMoves(int square);
uint64_t getQueenMoves(int square, uint64_t occupied);
bool isSquareAttacked(int square, Move::Color attackingColor);
bool isInCheck(Move::Color color);
uint64_t getCastlingMoves(Move::Color color, uint64_t occupied);

// Shifts
std::array<int, 64> rookShifts;
std::array<int, 64> bishopShifts;

// Attack Tables
std::vector<uint64_t> rookAttackTable;
std::vector<uint64_t> bishopAttackTable;

int main() {
    initializeBitboards();
    initializeMagicBitboards();
    bool gameOver = false;
    Move currentMove;
    Move::Color currentPlayer = Move::Color::WHITE;

    while (!gameOver) {
        // Display board
        std::cout << "\nCurrent Board State:\n";
        displayBoard();

        std::string moveStr;
        std::cout << "\n" << (currentPlayer == Move::Color::WHITE ? "White" : "Black") << " to move.\n";
        std::cout << "Enter move (e.g., 'a2 a4') or command ('moves e2', 'quit'): ";
        std::getline(std::cin, moveStr);

        if (moveStr == "quit") {
            gameOver = true;
            continue;
        }

        // Handle 'moves' command
        if (moveStr.substr(0, 5) == "moves") {
            if (moveStr.length() != 8) {
                std::cout << "Invalid format. Use 'moves e2'\n";
                continue;
            }
            
            int file = moveStr[6] - 'a';
            int rank = moveStr[7] - '1';
            
            if (file < 0 || file > 7 || rank < 0 || rank > 7) {
                std::cout << "Invalid square.\n";
                continue;
            }

            int square = rank * 8 + file;
            displayLegalMoves(square, currentPlayer);
            continue;
        }

        // Handle move input
        if (moveStr.length() != 5 || moveStr[2] != ' ') {
            std::cout << "Invalid move format. Use 'e2 e4'\n";
            continue;
        }

        int fromFile = moveStr[0] - 'a';
        int fromRank = moveStr[1] - '1';
        int toFile = moveStr[3] - 'a';
        int toRank = moveStr[4] - '1';

        if (fromFile < 0 || fromFile > 7 || fromRank < 0 || fromRank > 7 ||
            toFile < 0 || toFile > 7 || toRank < 0 || toRank > 7) {
            std::cout << "Invalid square coordinates.\n";
            continue;
        }

        currentMove.fromSquare = fromRank * 8 + fromFile;
        currentMove.toSquare = toRank * 8 + toFile;
        currentMove.color = currentPlayer;

        // Validate piece ownership
        uint64_t ownPieces = (currentPlayer == Move::Color::WHITE) ? whitePieces : blackPieces;
        if (!getBit(ownPieces, currentMove.fromSquare)) {
            std::cout << "No " << (currentPlayer == Move::Color::WHITE ? "white" : "black") 
                     << " piece on that square.\n";
            continue;
        }

        // Detect piece type and validate move
        bool validMove = false;
        uint64_t legalMoves = 0ULL;
        uint64_t occupied = whitePieces | blackPieces;

        if (getBit(whitePawns | blackPawns, currentMove.fromSquare)) {
            currentMove.pieceType = Move::PieceType::PAWN;
            legalMoves = getPawnMoves(currentMove.fromSquare, currentPlayer, occupied);
        } 
        else if (getBit(whiteRooks | blackRooks, currentMove.fromSquare)) {
            currentMove.pieceType = Move::PieceType::ROOK;
            legalMoves = getRookMoves(currentMove.fromSquare, occupied);
        }
        else if (getBit(whiteBishops | blackBishops, currentMove.fromSquare)) {
            currentMove.pieceType = Move::PieceType::BISHOP;
            legalMoves = getBishopMoves(currentMove.fromSquare, occupied);
        }
        else if (getBit(whiteQueens | blackQueens, currentMove.fromSquare)) {
            currentMove.pieceType = Move::PieceType::QUEEN;
            legalMoves = getQueenMoves(currentMove.fromSquare, occupied);
        }
        else if (getBit(whiteKnights | blackKnights, currentMove.fromSquare)) {
            currentMove.pieceType = Move::PieceType::KNIGHT;
            legalMoves = getKnightMoves(currentMove.fromSquare);
        }
        else if (getBit(whiteKing | blackKing, currentMove.fromSquare)) {
            currentMove.pieceType = Move::PieceType::KING;
            legalMoves = getKingMoves(currentMove.fromSquare);
        }

        // Remove moves that capture own pieces
        legalMoves &= ~ownPieces;

        if (!getBit(legalMoves, currentMove.toSquare)) {
            std::cout << "Illegal move for that piece.\n";
            continue;
        }

        // Check if move captures opponent's piece
        uint64_t opponentPieces = (currentPlayer == Move::Color::WHITE) ? blackPieces : whitePieces;
        currentMove.isCapture = getBit(opponentPieces, currentMove.toSquare);

        // Add castling check here
        if (currentMove.pieceType == Move::PieceType::KING) {
            if (currentPlayer == Move::Color::WHITE) {
                if (currentMove.fromSquare == 4 && currentMove.toSquare == 6) {
                    // White kingside castling
                    currentMove.isCastling = true;
                    currentMove.castlingType = Move::CastlingType::KINGSIDE;
                    // Move the rook too
                    whiteRooks = clearBit(whiteRooks, 7);  // Clear H1
                    whiteRooks = setBit(whiteRooks, 5);    // Set F1
                    castlingRights.whiteKingside = false;
                    castlingRights.whiteQueenside = false;
                } else if (currentMove.fromSquare == 4 && currentMove.toSquare == 2) {
                    // White queenside castling
                    currentMove.isCastling = true;
                    currentMove.castlingType = Move::CastlingType::QUEENSIDE;
                    // Move the rook too
                    whiteRooks = clearBit(whiteRooks, 0);  // Clear A1
                    whiteRooks = setBit(whiteRooks, 3);    // Set D1
                    castlingRights.whiteKingside = false;
                    castlingRights.whiteQueenside = false;
                }
            } else {
                if (currentMove.fromSquare == 60 && currentMove.toSquare == 62) {
                    // Black kingside castling
                    currentMove.isCastling = true;
                    currentMove.castlingType = Move::CastlingType::KINGSIDE;
                    // Move the rook too
                    blackRooks = clearBit(blackRooks, 63); // Clear H8
                    blackRooks = setBit(blackRooks, 61);   // Set F8
                    castlingRights.blackKingside = false;
                    castlingRights.blackQueenside = false;
                } else if (currentMove.fromSquare == 60 && currentMove.toSquare == 58) {
                    // Black queenside castling
                    currentMove.isCastling = true;
                    currentMove.castlingType = Move::CastlingType::QUEENSIDE;
                    // Move the rook too
                    blackRooks = clearBit(blackRooks, 56); // Clear A8
                    blackRooks = setBit(blackRooks, 59);   // Set D8
                    castlingRights.blackKingside = false;
                    castlingRights.blackQueenside = false;
                }
            }
        }

        // Add check for pawn promotion
        if (currentMove.pieceType == Move::PieceType::PAWN) {
            if ((currentMove.color == Move::Color::WHITE && currentMove.toSquare >= 56) ||
                (currentMove.color == Move::Color::BLACK && currentMove.toSquare < 8)) {
                currentMove.isPromotion = true;
                std::cout << "Choose promotion piece (Q/R/B/N): ";
                char choice;
                std::cin >> choice;
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');  // Clear input buffer
                
                switch (toupper(choice)) {
                    case 'Q': currentMove.promotionPiece = Move::PieceType::QUEEN; break;
                    case 'R': currentMove.promotionPiece = Move::PieceType::ROOK; break;
                    case 'B': currentMove.promotionPiece = Move::PieceType::BISHOP; break;
                    case 'N': currentMove.promotionPiece = Move::PieceType::KNIGHT; break;
                    default: currentMove.promotionPiece = Move::PieceType::QUEEN; break;
                }
            }
        }

        // Make the move
        updateBitboards(currentMove);

        // Switch player
        currentPlayer = (currentPlayer == Move::Color::WHITE) ? Move::Color::BLACK : Move::Color::WHITE;
    }

    std::cout << "Game Over.\n";
    return 0;
}

// helper functions
void displayBoard() {
    for (int rank = 7; rank >= 0; --rank) {
        std::cout << (rank + 1) << " ";
        for (int file = 0; file < 8; ++file) {
            int square = rank * 8 + file;
            if (getBit(whitePawns, square)) std::cout << "♟ ";
            else if (getBit(whiteKnights, square)) std::cout << "♞ ";
            else if (getBit(whiteBishops, square)) std::cout << "♝ ";
            else if (getBit(whiteRooks, square)) std::cout << "♜ ";
            else if (getBit(whiteQueens, square)) std::cout << "♛ ";
            else if (getBit(whiteKing, square)) std::cout << "♚ ";
            else if (getBit(blackPawns, square)) std::cout << "♙ ";
            else if (getBit(blackKnights, square)) std::cout << "♘ ";
            else if (getBit(blackBishops, square)) std::cout << "♗ ";
            else if (getBit(blackRooks, square)) std::cout << "♖ ";
            else if (getBit(blackQueens, square)) std::cout << "♕ ";
            else if (getBit(blackKing, square)) std::cout << "♔ ";
            else std::cout << ". ";
        }
        std::cout << std::endl;
    }
    std::cout << "  a b c d e f g h" << std::endl;
}

void displayLegalMoves(int square, Move::Color color) {
    uint64_t moves = 0ULL;
    uint64_t occupied = whitePieces | blackPieces;
    uint64_t ownPieces = (color == Move::Color::WHITE) ? whitePieces : blackPieces;

    // Get moves based on piece type
    if (getBit(whitePawns | blackPawns, square))
        moves = getPawnMoves(square, color, occupied);
    else if (getBit(whiteRooks | blackRooks, square))
        moves = getRookMoves(square, occupied);
    else if (getBit(whiteBishops | blackBishops, square))
        moves = getBishopMoves(square, occupied);
    else if (getBit(whiteQueens | blackQueens, square))
        moves = getQueenMoves(square, occupied);
    else if (getBit(whiteKnights | blackKnights, square))
        moves = getKnightMoves(square);
    else if (getBit(whiteKing | blackKing, square))
        moves = getKingMoves(square);

    // Remove moves that capture own pieces
    moves &= ~ownPieces;

    // Display the board with legal moves marked
    for (int rank = 7; rank >= 0; --rank) {
        std::cout << (rank + 1) << " ";
        for (int file = 0; file < 8; ++file) {
            int currentSquare = rank * 8 + file;
            if (getBit(moves, currentSquare))
                std::cout << "x ";
            else if (currentSquare == square)
                std::cout << "O ";
            else if (getBit(occupied, currentSquare))
                std::cout << "* ";
            else
                std::cout << ". ";
        }
        std::cout << std::endl;
    }
    std::cout << "  a b c d e f g h" << std::endl;
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

uint64_t generateRookMask(int square) {
    uint64_t mask = 0ULL;
    int rank = square / 8;
    int file = square % 8;

    // Generate rays in all 4 directions (excluding edges)
    for (int r = rank + 1; r < 7; r++) mask |= (1ULL << (r * 8 + file));
    for (int r = rank - 1; r > 0; r--) mask |= (1ULL << (r * 8 + file));
    for (int f = file + 1; f < 7; f++) mask |= (1ULL << (rank * 8 + f));
    for (int f = file - 1; f > 0; f--) mask |= (1ULL << (rank * 8 + f));

    return mask;
}

uint64_t generateBishopMask(int square) {
    uint64_t mask = 0ULL;
    int rank = square / 8;
    int file = square % 8;

    // Generate rays in all 4 diagonal directions (excluding edges)
    for (int r = rank + 1, f = file + 1; r < 7 && f < 7; r++, f++)
        mask |= (1ULL << (r * 8 + f));
    for (int r = rank + 1, f = file - 1; r < 7 && f > 0; r++, f--)
        mask |= (1ULL << (r * 8 + f));
    for (int r = rank - 1, f = file + 1; r > 0 && f < 7; r--, f++)
        mask |= (1ULL << (r * 8 + f));
    for (int r = rank - 1, f = file - 1; r > 0 && f > 0; r--, f--)
        mask |= (1ULL << (r * 8 + f));

    return mask;
}

uint64_t generateRookAttacks(int square, uint64_t blockers) {
    uint64_t attacks = 0ULL;
    int rank = square / 8;
    int file = square % 8;

    // Generate attacks in all 4 directions (stopping at blockers)
    for (int r = rank + 1; r < 8; r++) {
        attacks |= (1ULL << (r * 8 + file));
        if (blockers & (1ULL << (r * 8 + file))) break;
    }
    for (int r = rank - 1; r >= 0; r--) {
        attacks |= (1ULL << (r * 8 + file));
        if (blockers & (1ULL << (r * 8 + file))) break;
    }
    for (int f = file + 1; f < 8; f++) {
        attacks |= (1ULL << (rank * 8 + f));
        if (blockers & (1ULL << (rank * 8 + f))) break;
    }
    for (int f = file - 1; f >= 0; f--) {
        attacks |= (1ULL << (rank * 8 + f));
        if (blockers & (1ULL << (rank * 8 + f))) break;
    }

    return attacks;
}

uint64_t generateBishopAttacks(int square, uint64_t blockers) {
    uint64_t attacks = 0ULL;
    int rank = square / 8;
    int file = square % 8;

    // Generate attacks in all 4 diagonal directions (stopping at blockers)
    for (int r = rank + 1, f = file + 1; r < 8 && f < 8; r++, f++) {
        attacks |= (1ULL << (r * 8 + f));
        if (blockers & (1ULL << (r * 8 + f))) break;
    }
    for (int r = rank + 1, f = file - 1; r < 8 && f >= 0; r++, f--) {
        attacks |= (1ULL << (r * 8 + f));
        if (blockers & (1ULL << (r * 8 + f))) break;
    }
    for (int r = rank - 1, f = file + 1; r >= 0 && f < 8; r--, f++) {
        attacks |= (1ULL << (r * 8 + f));
        if (blockers & (1ULL << (r * 8 + f))) break;
    }
    for (int r = rank - 1, f = file - 1; r >= 0 && f >= 0; r--, f--) {
        attacks |= (1ULL << (r * 8 + f));
        if (blockers & (1ULL << (r * 8 + f))) break;
    }

    return attacks;
}

// Function to initialize the magic bitboard tables
void initializeMagicBitboards() {
    // Initialize rook tables
    for (int square = 0; square < 64; square++) {
        rookTable[square].mask = generateRookMask(square);
        rookTable[square].magic = ROOK_MAGICS[square];
        rookTable[square].shift = 64 - __builtin_popcountll(rookTable[square].mask);
        
        // Initialize attack table
        uint64_t mask = rookTable[square].mask;
        int variations = 1 << __builtin_popcountll(mask);
        rookTable[square].attacks.resize(variations);
        
        // Generate all possible blocker configurations
        for (int i = 0; i < variations; i++) {
            uint64_t blockers = 0ULL;
            int bit = 0;
            for (int square2 = 0; square2 < 64; square2++) {
                if (mask & (1ULL << square2)) {
                    if (i & (1 << bit)) {
                        blockers |= (1ULL << square2);
                    }
                    bit++;
                }
            }
            
            // Calculate the magic index
            uint64_t index = ((blockers * rookTable[square].magic) >> rookTable[square].shift);
            rookTable[square].attacks[index] = generateRookAttacks(square, blockers);
        }
    }

    // Initialize bishop tables (similar to rooks)
    for (int square = 0; square < 64; square++) {
        bishopTable[square].mask = generateBishopMask(square);
        bishopTable[square].magic = BISHOP_MAGICS[square];
        bishopTable[square].shift = 64 - __builtin_popcountll(bishopTable[square].mask);
        
        uint64_t mask = bishopTable[square].mask;
        int variations = 1 << __builtin_popcountll(mask);
        bishopTable[square].attacks.resize(variations);
        
        for (int i = 0; i < variations; i++) {
            uint64_t blockers = 0ULL;
            int bit = 0;
            for (int square2 = 0; square2 < 64; square2++) {
                if (mask & (1ULL << square2)) {
                    if (i & (1 << bit)) {
                        blockers |= (1ULL << square2);
                    }
                    bit++;
                }
            }
            
            uint64_t index = ((blockers * bishopTable[square].magic) >> bishopTable[square].shift);
            bishopTable[square].attacks[index] = generateBishopAttacks(square, blockers);
        }
    }
}

// Functions to get moves using magic bitboards
uint64_t getRookMoves(int square, uint64_t occupied) {
    MagicEntry& entry = rookTable[square];
    uint64_t blockers = occupied & entry.mask;
    int index = ((blockers * entry.magic) >> entry.shift);
    return entry.attacks[index];
}

uint64_t getBishopMoves(int square, uint64_t occupied) {
    MagicEntry& entry = bishopTable[square];
    uint64_t blockers = occupied & entry.mask;
    int index = ((blockers * entry.magic) >> entry.shift);
    return entry.attacks[index];
}

uint64_t getQueenMoves(int square, uint64_t occupied) {
    return getRookMoves(square, occupied) | getBishopMoves(square, occupied);
}

uint64_t getPawnMoves(int square, Move::Color color, uint64_t occupied) {
    uint64_t moves = 0ULL;
    int rank = square / 8;
    int file = square % 8;
    bool isPromotionRank = (color == Move::Color::WHITE && rank == 6) ||
                           (color == Move::Color::BLACK && rank == 1);

    if (color == Move::Color::WHITE) {
        // Single push
        if (rank < 7 && !getBit(occupied, square + 8)) {
            moves |= (1ULL << (square + 8));
            
            // Double push
            if (rank == 1 && !getBit(occupied, square + 16))
                moves |= (1ULL << (square + 16));
        }
        
        // Captures
        if (file > 0 && rank < 7 && getBit(blackPieces, square + 7))
            moves |= (1ULL << (square + 7));
        if (file < 7 && rank < 7 && getBit(blackPieces, square + 9))
            moves |= (1ULL << (square + 9));
    } else {
        // Single push
        if (rank > 0 && !getBit(occupied, square - 8))
            moves |= (1ULL << (square - 8));
        
        // Double push
        if (rank == 6 && !getBit(occupied, square - 8) && !getBit(occupied, square - 16))
            moves |= (1ULL << (square - 16));
        
        // Captures
        if (file > 0 && rank > 0 && getBit(whitePieces, square - 9))
            moves |= (1ULL << (square - 9));
        if (file < 7 && rank > 0 && getBit(whitePieces, square - 7))
            moves |= (1ULL << (square - 7));
    }

    return moves;
}

uint64_t getKnightMoves(int square) {
    uint64_t moves = 0ULL;
    int rank = square / 8;
    int file = square % 8;

    const int knightMoves[8][2] = {
        {-2, -1}, {-2, 1}, {-1, -2}, {-1, 2},
        {1, -2}, {1, 2}, {2, -1}, {2, 1}
    };

    for (const auto& move : knightMoves) {
        int newRank = rank + move[0];
        int newFile = file + move[1];
        
        if (newRank >= 0 && newRank < 8 && newFile >= 0 && newFile < 8) {
            moves |= (1ULL << (newRank * 8 + newFile));
        }
    }

    return moves;
}

uint64_t getKingMoves(int square) {
    uint64_t moves = 0ULL;
    int rank = square / 8;
    int file = square % 8;

    const int kingMoves[8][2] = {
        {-1, -1}, {-1, 0}, {-1, 1},
        {0, -1}, {0, 1},
        {1, -1}, {1, 0}, {1, 1}
    };

    for (const auto& move : kingMoves) {
        int newRank = rank + move[0];
        int newFile = file + move[1];
        
        if (newRank >= 0 && newRank < 8 && newFile >= 0 && newFile < 8) {
            moves |= (1ULL << (newRank * 8 + newFile));
        }
    }

    return moves;
}

bool isSquareAttacked(int square, Move::Color attackingColor) {
    uint64_t occupied = whitePieces | blackPieces;
    uint64_t attackers = (attackingColor == Move::Color::WHITE) ? whitePieces : blackPieces;
    
    // Check for pawn attacks
    if (attackingColor == Move::Color::WHITE) {
        if (square > 7 && ((square % 8) > 0 && getBit(whitePawns, square - 9) ||
            (square % 8) < 7 && getBit(whitePawns, square - 7))) {
            return true;
        }
    } else {
        if (square < 56 && ((square % 8) > 0 && getBit(blackPawns, square + 7) ||
            (square % 8) < 7 && getBit(blackPawns, square + 9))) {
            return true;
        }
    }

    // Check for knight attacks
    uint64_t knightAttacks = getKnightMoves(square);
    if (knightAttacks & (attackingColor == Move::Color::WHITE ? whiteKnights : blackKnights))
        return true;

    // Check for bishop/queen attacks
    uint64_t bishopAttacks = getBishopMoves(square, occupied);
    if (bishopAttacks & (attackingColor == Move::Color::WHITE ? 
        (whiteBishops | whiteQueens) : (blackBishops | blackQueens)))
        return true;

    // Check for rook/queen attacks
    uint64_t rookAttacks = getRookMoves(square, occupied);
    if (rookAttacks & (attackingColor == Move::Color::WHITE ? 
        (whiteRooks | whiteQueens) : (blackRooks | blackQueens)))
        return true;

    // Check for king attacks
    uint64_t kingAttacks = getKingMoves(square);
    if (kingAttacks & (attackingColor == Move::Color::WHITE ? whiteKing : blackKing))
        return true;

    return false;
}

bool isInCheck(Move::Color color) {
    int kingSquare = -1;
    uint64_t kingBB = (color == Move::Color::WHITE) ? whiteKing : blackKing;
    
    // Find king's square
    for (int sq = 0; sq < 64; sq++) {
        if (getBit(kingBB, sq)) {
            kingSquare = sq;
            break;
        }
    }
    
    return isSquareAttacked(kingSquare, color == Move::Color::WHITE ? Move::Color::BLACK : Move::Color::WHITE);
}

uint64_t getCastlingMoves(Move::Color color, uint64_t occupied) {
    uint64_t moves = 0ULL;
    
    if (color == Move::Color::WHITE) {
        if (castlingRights.whiteKingside) {
            if (!getBit(occupied, 5) && !getBit(occupied, 6) &&
                !isSquareAttacked(4, Move::Color::BLACK) &&
                !isSquareAttacked(5, Move::Color::BLACK) &&
                !isSquareAttacked(6, Move::Color::BLACK)) {
                moves |= (1ULL << 6);  // g1
            }
        }
        if (castlingRights.whiteQueenside) {
            if (!getBit(occupied, 1) && !getBit(occupied, 2) && !getBit(occupied, 3) &&
                !isSquareAttacked(4, Move::Color::BLACK) &&
                !isSquareAttacked(3, Move::Color::BLACK) &&
                !isSquareAttacked(2, Move::Color::BLACK)) {
                moves |= (1ULL << 2);  // c1
            }
        }
    } else {
        if (castlingRights.blackKingside) {
            if (!getBit(occupied, 61) && !getBit(occupied, 62) &&
                !isSquareAttacked(60, Move::Color::WHITE) &&
                !isSquareAttacked(61, Move::Color::WHITE) &&
                !isSquareAttacked(62, Move::Color::WHITE)) {
                moves |= (1ULL << 62);  // g8
            }
        }
        if (castlingRights.blackQueenside) {
            if (!getBit(occupied, 57) && !getBit(occupied, 58) && !getBit(occupied, 59) &&
                !isSquareAttacked(60, Move::Color::WHITE) &&
                !isSquareAttacked(59, Move::Color::WHITE) &&
                !isSquareAttacked(58, Move::Color::WHITE)) {
                moves |= (1ULL << 58);  // c8
            }
        }
    }
    
    return moves;
}

// end of file