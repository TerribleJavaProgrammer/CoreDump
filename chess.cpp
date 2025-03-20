/*
* Chess engine
* Created on: 3-6-25
*/

// Header files
#include <stdlib.h>
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

constexpr uint64_t KNIGHT_ATTACKS[64] = {
    0x0000000000020400ULL, 0x0000000000050800ULL, 0x00000000000a1100ULL, 0x0000000000142200ULL,
    0x0000000000284400ULL, 0x0000000000508800ULL, 0x0000000000a01000ULL, 0x0000000000402000ULL,
    0x0000000002040004ULL, 0x0000000005080008ULL, 0x000000000a110011ULL, 0x0000000014220022ULL,
    0x0000000028440044ULL, 0x0000000050880088ULL, 0x00000000a0100010ULL, 0x0000000040200020ULL,
    0x0000000204000402ULL, 0x0000000508000805ULL, 0x0000000a1100110aULL, 0x0000001422002214ULL,
    0x0000002844004428ULL, 0x0000005088008850ULL, 0x000000a0100010a0ULL, 0x0000004020002040ULL,
    0x0000020400040200ULL, 0x0000050800080500ULL, 0x00000a1100110a00ULL, 0x0000142200221400ULL,
    0x0000284400442800ULL, 0x0000508800885000ULL, 0x0000a0100010a000ULL, 0x0000402000204000ULL,
    0x0002040004020000ULL, 0x0005080008050000ULL, 0x000a1100110a0000ULL, 0x0014220022140000ULL,
    0x0028440044280000ULL, 0x0050880088500000ULL, 0x00a0100010a00000ULL, 0x0040200020400000ULL,
    0x0204000402000000ULL, 0x0508000805000000ULL, 0x0a1100110a000000ULL, 0x1422002214000000ULL,
    0x2844004428000000ULL, 0x5088008850000000ULL, 0xa0100010a0000000ULL, 0x4020002040000000ULL,
    0x0400040200000000ULL, 0x0800080500000000ULL, 0x1100110a00000000ULL, 0x2200221400000000ULL,
    0x4400442800000000ULL, 0x8800885000000000ULL, 0x100010a000000000ULL, 0x2000204000000000ULL,
    0x0004020000000000ULL, 0x0008050000000000ULL, 0x00110a0000000000ULL, 0x0022140000000000ULL,
    0x0044280000000000ULL, 0x0088500000000000ULL, 0x0010a00000000000ULL, 0x0020400000000000ULL
};

constexpr uint64_t KING_ATTACKS[64] = {
    0x0000000000000302ULL, 0x0000000000000705ULL, 0x0000000000000E0AULL, 0x0000000000001C14ULL,
    0x0000000000003828ULL, 0x0000000000007050ULL, 0x000000000000E0A0ULL, 0x000000000000C040ULL,
    0x0000000000030203ULL, 0x0000000000070507ULL, 0x00000000000E0A0EULL, 0x00000000001C141CULL,
    0x0000000000382838ULL, 0x0000000000705070ULL, 0x0000000000E0A0E0ULL, 0x0000000000C040C0ULL,
    0x0000000003020300ULL, 0x0000000007050700ULL, 0x000000000E0A0E00ULL, 0x000000001C141C00ULL,
    0x0000000038283800ULL, 0x0000000070507000ULL, 0x00000000E0A0E000ULL, 0x00000000C040C000ULL,
    0x0000000302030000ULL, 0x0000000705070000ULL, 0x0000000E0A0E0000ULL, 0x0000001C141C0000ULL,
    0x0000003828380000ULL, 0x0000007050700000ULL, 0x000000E0A0E00000ULL, 0x000000C040C00000ULL,
    0x0000030203000000ULL, 0x0000070507000000ULL, 0x00000E0A0E000000ULL, 0x00001C141C000000ULL,
    0x0000382838000000ULL, 0x0000705070000000ULL, 0x0000E0A0E0000000ULL, 0x0000C040C0000000ULL,
    0x0003020300000000ULL, 0x0007050700000000ULL, 0x000E0A0E00000000ULL, 0x001C141C00000000ULL,
    0x0038283800000000ULL, 0x0070507000000000ULL, 0x00E0A0E000000000ULL, 0x00C040C000000000ULL,
    0x0302030000000000ULL, 0x0705070000000000ULL, 0x0E0A0E0000000000ULL, 0x1C141C0000000000ULL,
    0x3828380000000000ULL, 0x7050700000000000ULL, 0xE0A0E00000000000ULL, 0xC040C00000000000ULL,
    0x0203000000000000ULL, 0x0507000000000000ULL, 0x0A0E000000000000ULL, 0x141C000000000000ULL,
    0x2838000000000000ULL, 0x5070000000000000ULL, 0xA0E0000000000000ULL, 0x40C0000000000000ULL
};

constexpr uint64_t FILE_A = 0x0101010101010101ULL;
constexpr uint64_t FILE_H = 0x8080808080808080ULL;

constexpr uint64_t RANK_2 = 0x000000000000FF00ULL;
constexpr uint64_t RANK_7 = 0x00FF000000000000ULL;
constexpr uint64_t RANK_4 = 0x00000000FF000000ULL;
constexpr uint64_t RANK_5 = 0x000000FF00000000ULL;
constexpr uint64_t RANK_1 = 0x00000000000000FFULL;
constexpr uint64_t RANK_8 = 0xFF00000000000000ULL;

// for magic bitboards
constexpr int BOARD_SIZE = 64;

// Material values
constexpr int PAWN_VALUE = 100;
constexpr int KNIGHT_VALUE = 320;
constexpr int BISHOP_VALUE = 330;
constexpr int ROOK_VALUE = 500;
constexpr int QUEEN_VALUE = 900;
constexpr int KING_VALUE = 20000;

// pst - edit to change behavior
constexpr std::array<int, 64> PAWN_PST = {
    0,  0,  0,  0,  0,  0,  0,  0,
    50, 50, 50, 50, 50, 50, 50, 50,
    10, 10, 20, 30, 30, 20, 10, 10,
    5,  5, 10, 25, 25, 10,  5,  5,
    0,  0,  0, 20, 20,  0,  0,  0,
    5, -5,-10,  0,  0,-10, -5,  5,
    5, 10, 10,-20,-20, 10, 10,  5,
    0,  0,  0,  0,  0,  0,  0,  0
};

constexpr std::array<int, 64> KNIGHT_PST = {
    -50,-40,-30,-30,-30,-30,-40,-50,
    -40,-20,  0,  0,  0,  0,-20,-40,
    -30,  0, 10, 15, 15, 10,  0,-30,
    -30,  5, 15, 20, 20, 15,  5,-30,
    -30,  0, 15, 20, 20, 15,  0,-30,
    -30,  5, 10, 15, 15, 10,  5,-30,
    -40,-20,  0,  5,  5,  0,-20,-40,
    -50,-40,-30,-30,-30,-30,-40,-50
};

constexpr std::array<int, 64> BISHOP_PST = {
    -20,-10,-10,-10,-10,-10,-10,-20,
    -10,  0,  0,  0,  0,  0,  0,-10,
    -10,  0,  5, 10, 10,  5,  0,-10,
    -10,  5,  5, 10, 10,  5,  5,-10,
    -10,  0, 10, 10, 10, 10,  0,-10,
    -10, 10, 10, 10, 10, 10, 10,-10,
    -10,  5,  0,  0,  0,  0,  5,-10,
    -20,-10,-10,-10,-10,-10,-10,-20
};

constexpr std::array<int, 64> ROOK_PST = {
    0,  0,  0,  0,  0,  0,  0,  0,
    5, 10, 10, 10, 10, 10, 10,  5,
    -5,  0,  0,  0,  0,  0,  0, -5,
    -5,  0,  0,  0,  0,  0,  0, -5,
    -5,  0,  0,  0,  0,  0,  0, -5,
    -5,  0,  0,  0,  0,  0,  0, -5,
    -5,  0,  0,  0,  0,  0,  0, -5,
    0,  0,  0,  5,  5,  0,  0,  0
};

constexpr std::array<int, 64> QUEEN_PST = {
    -20,-10,-10, -5, -5,-10,-10,-20,
    -10,  0,  0,  0,  0,  0,  0,-10,
    -10,  0,  5,  5,  5,  5,  0,-10,
    -5,  0,  5,  5,  5,  5,  0, -5,
    0,  0,  5,  5,  5,  5,  0, -5,
    -10,  5,  5,  5,  5,  5,  0,-10,
    -10,  0,  5,  0,  0,  0,  0,-10,
    -20,-10,-10, -5, -5,-10,-10,-20
};

constexpr std::array<int, 64> KING_PST = {
    -30,-40,-40,-50,-50,-40,-40,-30,
    -30,-40,-40,-50,-50,-40,-40,-30,
    -30,-40,-40,-50,-50,-40,-40,-30,
    -30,-40,-40,-50,-50,-40,-40,-30,
    -20,-30,-30,-40,-40,-30,-30,-20,
    -10,-20,-20,-20,-20,-20,-20,-10,
    20, 20,  0,  0,  0,  0, 20, 20,
    20, 30, 10,  0,  0, 10, 30, 20
};

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
struct Position {
    uint64_t whitePawns, whiteKnights, whiteBishops, whiteRooks, whiteQueens, whiteKing;
    uint64_t blackPawns, blackKnights, blackBishops, blackRooks, blackQueens, blackKing;
    uint64_t whitePieces, blackPieces, occupiedSquares, emptySquares;
    CastlingRights castling;
};

CastlingRights castlingRights;

// Initialize magic lookup tables
std::array<MagicEntry, 64> rookTable;
std::array<MagicEntry, 64> bishopTable;

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
int evaluatePosition();
int negamax(int depth, int alpha, int beta, Move::Color color);
std::vector<Move> generateMoves(Move::Color color);
void sortMoves(std::vector<Move>& moves);

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
    Move::Color currentPlayer = Move::Color::WHITE;
    bool playerIsWhite = true; // True if human plays white

    std::cout << "Play as (w)hite or (b)lack? ";
    char choice;
    std::cin >> choice;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    playerIsWhite = (tolower(choice) != 'b');

    while (!gameOver) {
        // Display current state
        std::cout << "\n====================\n";
        std::cout << (currentPlayer == Move::Color::WHITE ? "White" : "Black") << " to move\n";
        if (isInCheck(currentPlayer)) {
            std::cout << "CHECK!\n";
        }
        displayBoard();

        bool isPlayerTurn = (currentPlayer == Move::Color::WHITE) == playerIsWhite;

        if (isPlayerTurn) {
            // Human move
            std::cout << "\nCommands:\n";
            std::cout << "- Move: e2 e4\n";
            std::cout << "- Show moves: moves e2\n";
            std::cout << "- Quit: quit\n> ";
            
            std::string input;
            std::getline(std::cin, input);

            if (input == "quit") {
                break;
            }

            if (input.substr(0, 5) == "moves") {
                if (input.length() != 8) {
                    std::cout << "Invalid format. Use 'moves e2'\n";
                    continue;
                }
                
                int file = input[6] - 'a';
                int rank = input[7] - '1';
                
                if (file < 0 || file > 7 || rank < 0 || rank > 7) {
                    std::cout << "Invalid square.\n";
                    continue;
                }

                int square = rank * 8 + file;
                displayLegalMoves(square, currentPlayer);
                continue;
            }

            // Process move
            if (input.length() != 5 || input[2] != ' ') {
                std::cout << "Invalid move format. Use 'e2 e4'\n";
                continue;
            }

            Move move;
            move.fromSquare = (input[1] - '1') * 8 + (input[0] - 'a');
            move.toSquare = (input[4] - '1') * 8 + (input[3] - 'a');
            move.color = currentPlayer;

            // Validate move
            std::vector<Move> legalMoves = generateMoves(currentPlayer);
            auto moveIt = std::find_if(legalMoves.begin(), legalMoves.end(),
                [&move](const Move& m) {
                    return m.fromSquare == move.fromSquare && m.toSquare == move.toSquare;
                });

            if (moveIt == legalMoves.end()) {
                std::cout << "Illegal move.\n";
                continue;
            }

            // Copy all move properties from found legal move
            move = *moveIt;

            // Handle promotion
            if (move.isPromotion) {
                std::cout << "Choose promotion piece (Q/R/B/N): ";
                char choice;
                std::cin >> choice;
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                
                switch (toupper(choice)) {
                    case 'Q': move.promotionPiece = Move::PieceType::QUEEN; break;
                    case 'R': move.promotionPiece = Move::PieceType::ROOK; break;
                    case 'B': move.promotionPiece = Move::PieceType::BISHOP; break;
                    case 'N': move.promotionPiece = Move::PieceType::KNIGHT; break;
                    default: move.promotionPiece = Move::PieceType::QUEEN; break;
                }
            }

            updateBitboards(move);
        } else {
            // Computer move
            std::cout << "Computer is thinking...\n";
            Move computerMove = findBestMove(currentPlayer, 4); // Search 4 ply deep
            
            // Convert move to algebraic notation for display
            std::string moveStr = 
                std::string(1, 'a' + (computerMove.fromSquare % 8)) +
                std::string(1, '1' + (computerMove.fromSquare / 8)) + " " +
                std::string(1, 'a' + (computerMove.toSquare % 8)) +
                std::string(1, '1' + (computerMove.toSquare / 8));
                
            std::cout << "Computer plays: " << moveStr << std::endl;
            
            updateBitboards(computerMove);
        }

        // Check for game end conditions
        std::vector<Move> nextMoves = generateMoves(currentPlayer);
        if (nextMoves.empty()) {
            if (isInCheck(currentPlayer)) {
                std::cout << "\nCheckmate! " 
                         << (currentPlayer == Move::Color::WHITE ? "Black" : "White") 
                         << " wins!\n";
            } else {
                std::cout << "\nStalemate! Game is a draw.\n";
            }
            gameOver = true;
            continue;
        }

        // Switch players
        currentPlayer = (currentPlayer == Move::Color::WHITE) ? 
            Move::Color::BLACK : Move::Color::WHITE;
    }

    std::cout << "\nGame ended.\n";
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
    uint64_t fromBB = 1ULL << move.fromSquare;
    uint64_t toBB = 1ULL << move.toSquare;
    bool isWhite = move.color == Move::Color::WHITE;
    
    // 1. Clear the piece from its original square
    switch (move.pieceType) {
        case Move::PieceType::PAWN:   
            isWhite ? whitePawns &= ~fromBB : blackPawns &= ~fromBB; break;
        case Move::PieceType::KNIGHT: 
            isWhite ? whiteKnights &= ~fromBB : blackKnights &= ~fromBB; break;
        case Move::PieceType::BISHOP: 
            isWhite ? whiteBishops &= ~fromBB : blackBishops &= ~fromBB; break;
        case Move::PieceType::ROOK:   
            isWhite ? whiteRooks &= ~fromBB : blackRooks &= ~fromBB; break;
        case Move::PieceType::QUEEN:  
            isWhite ? whiteQueens &= ~fromBB : blackQueens &= ~fromBB; break;
        case Move::PieceType::KING:   
            isWhite ? whiteKing &= ~fromBB : blackKing &= ~fromBB; break;
    }

    // 2. Handle captures by clearing captured piece
    if (move.isCapture) {
        uint64_t* targetBoards[] = {
            isWhite ? &blackPawns : &whitePawns,
            isWhite ? &blackKnights : &whiteKnights,
            isWhite ? &blackBishops : &whiteBishops,
            isWhite ? &blackRooks : &whiteRooks,
            isWhite ? &blackQueens : &whiteQueens,
            isWhite ? &blackKing : &whiteKing
        };
        
        for (uint64_t* board : targetBoards) {
            if (*board & toBB) {
                *board &= ~toBB;
                break;
            }
        }
    }

    // 3. Place the piece on its new square
    if (move.isPromotion) {
        // Handle promotion
        switch (move.promotionPiece) {
            case Move::PieceType::QUEEN:
                isWhite ? whiteQueens |= toBB : blackQueens |= toBB; break;
            case Move::PieceType::ROOK:
                isWhite ? whiteRooks |= toBB : blackRooks |= toBB; break;
            case Move::PieceType::BISHOP:
                isWhite ? whiteBishops |= toBB : blackBishops |= toBB; break;
            case Move::PieceType::KNIGHT:
                isWhite ? whiteKnights |= toBB : blackKnights |= toBB; break;
        }
    } else {
        // Normal piece placement
        switch (move.pieceType) {
            case Move::PieceType::PAWN:
                isWhite ? whitePawns |= toBB : blackPawns |= toBB; break;
            case Move::PieceType::KNIGHT:
                isWhite ? whiteKnights |= toBB : blackKnights |= toBB; break;
            case Move::PieceType::BISHOP:
                isWhite ? whiteBishops |= toBB : blackBishops |= toBB; break;
            case Move::PieceType::ROOK:
                isWhite ? whiteRooks |= toBB : blackRooks |= toBB; break;
            case Move::PieceType::QUEEN:
                isWhite ? whiteQueens |= toBB : blackQueens |= toBB; break;
            case Move::PieceType::KING:
                isWhite ? whiteKing |= toBB : blackKing |= toBB; break;
        }
    }

    // 4. Handle castling rook moves
    if (move.isCastling) {
        if (isWhite) {
            if (move.castlingType == Move::CastlingType::KINGSIDE) {
                whiteRooks &= ~(1ULL << 7);  // Clear H1
                whiteRooks |= (1ULL << 5);   // Set F1
            } else {
                whiteRooks &= ~(1ULL << 0);  // Clear A1
                whiteRooks |= (1ULL << 3);   // Set D1
            }
        } else {
            if (move.castlingType == Move::CastlingType::KINGSIDE) {
                blackRooks &= ~(1ULL << 63);  // Clear H8
                blackRooks |= (1ULL << 61);   // Set F8
            } else {
                blackRooks &= ~(1ULL << 56);  // Clear A8
                blackRooks |= (1ULL << 59);   // Set D8
            }
        }
    }

    // 5. Update composite bitboards
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
    uint64_t pawnBB = 1ULL << square;

    if (color == Move::Color::WHITE) {
        // Single pushes
        uint64_t singlePush = (pawnBB << 8) & ~occupied;
        moves |= singlePush;
        
        if (pawnBB & RANK_2) {
            moves |= ((singlePush << 8) & ~occupied & RANK_4);
        }
        if ((square % 8) != 0) {
            moves |= ((pawnBB << 7) & blackPieces & ~FILE_H);
        }
        if ((square % 8) != 7) {
            moves |= ((pawnBB << 9) & blackPieces & ~FILE_A);
        }
        
    } else {
        uint64_t singlePush = (pawnBB >> 8) & ~occupied;
        moves |= singlePush;
        
        if (pawnBB & RANK_7) {
            moves |= ((singlePush >> 8) & ~occupied & RANK_5);
        }
        
        // Captures
        if ((square % 8) != 0) {
            moves |= ((pawnBB >> 9) & whitePieces & ~FILE_H);
        }
        if ((square % 8) != 7) {
            moves |= ((pawnBB >> 7) & whitePieces & ~FILE_A);
        }
    }

    return moves;
}

uint64_t getKnightMoves(int square) {
    return KNIGHT_ATTACKS[square];
}

uint64_t getKingMoves(int square) {
    return KING_ATTACKS[square];
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
    uint64_t kingBB = (color == Move::Color::WHITE) ? whiteKing : blackKing;
    int kingSquare = __builtin_ctzll(kingBB);
    return isSquareAttacked(kingSquare, color == Move::Color::WHITE ? 
        Move::Color::BLACK : Move::Color::WHITE);
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

Position saveBitboards() {
    Position pos;
    pos.whitePawns = whitePawns;
    pos.whiteKnights = whiteKnights;
    pos.whiteBishops = whiteBishops;
    pos.whiteRooks = whiteRooks;
    pos.whiteQueens = whiteQueens;
    pos.whiteKing = whiteKing;
    
    pos.blackPawns = blackPawns;
    pos.blackKnights = blackKnights;
    pos.blackBishops = blackBishops;
    pos.blackRooks = blackRooks;
    pos.blackQueens = blackQueens;
    pos.blackKing = blackKing;
    
    pos.whitePieces = whitePieces;
    pos.blackPieces = blackPieces;
    pos.occupiedSquares = occupiedSquares;
    pos.emptySquares = emptySquares;
    pos.castling = castlingRights;
    
    return pos;
}

void restoreBitboards(const Position& pos) {
    whitePawns = pos.whitePawns;
    whiteKnights = pos.whiteKnights;
    whiteBishops = pos.whiteBishops;
    whiteRooks = pos.whiteRooks;
    whiteQueens = pos.whiteQueens;
    whiteKing = pos.whiteKing;
    
    blackPawns = pos.blackPawns;
    blackKnights = pos.blackKnights;
    blackBishops = pos.blackBishops;
    blackRooks = pos.blackRooks;
    blackQueens = pos.blackQueens;
    blackKing = pos.blackKing;
    
    whitePieces = pos.whitePieces;
    blackPieces = pos.blackPieces;
    occupiedSquares = pos.occupiedSquares;
    emptySquares = pos.emptySquares;
    castlingRights = pos.castling;
}

int evaluatePosition() {
    int score = 0;
    uint64_t occupied = whitePieces | blackPieces;
    
    // Single pass through the board
    for (int square = 0; square < 64; square++) {
        // White pieces
        if (getBit(whitePawns, square)) {
            score += PAWN_VALUE + PAWN_PST[square];
        }
        else if (getBit(whiteKnights, square)) {
            score += KNIGHT_VALUE + KNIGHT_PST[square];
        }
        else if (getBit(whiteBishops, square)) {
            score += BISHOP_VALUE + BISHOP_PST[square];
        }
        else if (getBit(whiteRooks, square)) {
            score += ROOK_VALUE + ROOK_PST[square];
        }
        else if (getBit(whiteQueens, square)) {
            score += QUEEN_VALUE + QUEEN_PST[square];
        }
        else if (getBit(whiteKing, square)) {
            score += KING_PST[square];  // King value not added to avoid huge scores
        }
        
        // Black pieces
        else if (getBit(blackPawns, square)) {
            score -= PAWN_VALUE + PAWN_PST[63 - square];
        }
        else if (getBit(blackKnights, square)) {
            score -= KNIGHT_VALUE + KNIGHT_PST[63 - square];
        }
        else if (getBit(blackBishops, square)) {
            score -= BISHOP_VALUE + BISHOP_PST[63 - square];
        }
        else if (getBit(blackRooks, square)) {
            score -= ROOK_VALUE + ROOK_PST[63 - square];
        }
        else if (getBit(blackQueens, square)) {
            score -= QUEEN_VALUE + QUEEN_PST[63 - square];
        }
        else if (getBit(blackKing, square)) {
            score -= KING_PST[63 - square];
        }
    }
    
    return score;
}

int negamax(int depth, int alpha, int beta, Move::Color color) {
    if (depth == 0) {
        return color == Move::Color::WHITE ? evaluatePosition() : -evaluatePosition();
    }
    
    std::vector<Move> moves = generateMoves(color);
    if (moves.empty()) {
        if (isInCheck(color)) {
            return -KING_VALUE;  // Checkmate
        }
        return 0;  // Stalemate
    }
    
    sortMoves(moves);  // Move ordering for better pruning
    
    int maxScore = -KING_VALUE * 2;
    for (const Move& move : moves) {
        // Make move
        Position oldPosition = saveBitboards();  // Changed from uint64_t
        updateBitboards(move);
        
        int score = -negamax(depth - 1, -beta, -alpha, 
            color == Move::Color::WHITE ? Move::Color::BLACK : Move::Color::WHITE);
        
        // Unmake move
        restoreBitboards(oldPosition);
        
        maxScore = std::max(maxScore, score);
        alpha = std::max(alpha, score);
        if (alpha >= beta) {
            break;  // Beta cutoff
        }
    }
    
    return maxScore;
}

std::vector<Move> generateMoves(Move::Color color) {
    std::vector<Move> moves;
    uint64_t pieces = (color == Move::Color::WHITE) ? whitePieces : blackPieces;
    uint64_t occupied = whitePieces | blackPieces;
    
    // Generate moves for each piece type
    for (int square = 0; square < 64; square++) {
        if (!getBit(pieces, square)) continue;
        
        uint64_t legalMoves = 0ULL;
        Move move;
        move.fromSquare = square;
        move.color = color;
        move.isCastling = false;
        move.isPromotion = false;
        
        // Pawns
        if (getBit(whitePawns | blackPawns, square)) {
            move.pieceType = Move::PieceType::PAWN;
            legalMoves = getPawnMoves(square, color, occupied);
            
            // Check for promotions
            if ((color == Move::Color::WHITE && square >= 48 && square < 56) ||
                (color == Move::Color::BLACK && square >= 8 && square < 16)) {
                move.isPromotion = true;
            }
        }
        // Knights
        else if (getBit(whiteKnights | blackKnights, square)) {
            move.pieceType = Move::PieceType::KNIGHT;
            legalMoves = getKnightMoves(square) & ~pieces;
        }
        // Bishops
        else if (getBit(whiteBishops | blackBishops, square)) {
            move.pieceType = Move::PieceType::BISHOP;
            legalMoves = getBishopMoves(square, occupied) & ~pieces;
        }
        // Rooks
        else if (getBit(whiteRooks | blackRooks, square)) {
            move.pieceType = Move::PieceType::ROOK;
            legalMoves = getRookMoves(square, occupied) & ~pieces;
        }
        // Queens
        else if (getBit(whiteQueens | blackQueens, square)) {
            move.pieceType = Move::PieceType::QUEEN;
            legalMoves = getQueenMoves(square, occupied) & ~pieces;
        }
        // Kings
        else if (getBit(whiteKing | blackKing, square)) {
            move.pieceType = Move::PieceType::KING;
            legalMoves = getKingMoves(square) & ~pieces;
            
            // Add castling moves
            uint64_t castlingMoves = getCastlingMoves(color, occupied);
            if (castlingMoves) {
                while (castlingMoves) {
                    int toSquare = __builtin_ctzll(castlingMoves);
                    Move castlingMove = move;
                    castlingMove.toSquare = toSquare;
                    castlingMove.isCastling = true;
                    castlingMove.castlingType = (toSquare % 8 > 4) ? 
                        Move::CastlingType::KINGSIDE : Move::CastlingType::QUEENSIDE;
                    moves.push_back(castlingMove);
                    castlingMoves &= castlingMoves - 1;
                }
            }
        }

        // Convert bitboard of legal moves to actual Move objects
        while (legalMoves) {
            int toSquare = __builtin_ctzll(legalMoves);
            move.toSquare = toSquare;
            move.isCapture = getBit((color == Move::Color::WHITE ? blackPieces : whitePieces), toSquare);
            
            if (move.isPromotion) {
                // Generate all possible promotion pieces
                Move promotionMove = move;
                promotionMove.promotionPiece = Move::PieceType::QUEEN;
                moves.push_back(promotionMove);
                promotionMove.promotionPiece = Move::PieceType::ROOK;
                moves.push_back(promotionMove);
                promotionMove.promotionPiece = Move::PieceType::BISHOP;
                moves.push_back(promotionMove);
                promotionMove.promotionPiece = Move::PieceType::KNIGHT;
                moves.push_back(promotionMove);
            } else {
                moves.push_back(move);
            }
            
            legalMoves &= legalMoves - 1;  // clear least significant bit
        }
    }
    
    return moves;
}

void sortMoves(std::vector<Move>& moves) {
    // Simple move ordering: captures first
    std::sort(moves.begin(), moves.end(), 
        [](const Move& a, const Move& b) {
            return a.isCapture > b.isCapture;
        });
}

Move findBestMove(Move::Color color, int depth) {
    std::vector<Move> moves = generateMoves(color);
    if (moves.empty()) return Move(); // Return invalid move if no moves available
    
    int bestScore = -KING_VALUE * 2;
    Move bestMove = moves[0];
    int alpha = -KING_VALUE * 2;
    int beta = KING_VALUE * 2;
    
    for (const Move& move : moves) {
        Position oldPosition = saveBitboards();
        updateBitboards(move);
        
        int score = -negamax(depth - 1, -beta, -alpha, 
            color == Move::Color::WHITE ? Move::Color::BLACK : Move::Color::WHITE);
            
        restoreBitboards(oldPosition);
        
        if (score > bestScore) {
            bestScore = score;
            bestMove = move;
        }
        alpha = std::max(alpha, score);
    }
    
    return bestMove;
}

// end of file