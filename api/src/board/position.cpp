#include "board/position.h"

namespace coredump
{
    char Position::getSquareChar(int square)
    {
        if (getBit(whitePawns, square))
            return 'P'; // white pawn
        if (getBit(whiteKnights, square))
            return 'N'; // white knight
        if (getBit(whiteBishops, square))
            return 'B'; // white bishop
        if (getBit(whiteRooks, square))
            return 'R'; // white rook
        if (getBit(whiteQueens, square))
            return 'Q'; // white queen
        if (getBit(whiteKing, square))
            return 'K'; // white king
        if (getBit(blackPawns, square))
            return 'p'; // black pawn
        if (getBit(blackKnights, square))
            return 'n'; // black knight
        if (getBit(blackBishops, square))
            return 'b'; // black bishop
        if (getBit(blackRooks, square))
            return 'r'; // black rook
        if (getBit(blackQueens, square))
            return 'q'; // black queen
        if (getBit(blackKing, square))
            return 'k'; // black king
        return '.';     // empty square
    }

    // Constructor
    Position::Position() : whitePawns(0), whiteKnights(0), whiteBishops(0), whiteRooks(0), whiteQueens(0), whiteKing(0),
                           blackPawns(0), blackKnights(0), blackBishops(0), blackRooks(0), blackQueens(0), blackKing(0),
                           castlingRights(0), enPassantSquare(-1)
    {
        // Initialize white pieces
        for (int i = 8; i < 16; ++i)
        {
            whitePawns = setBit(whitePawns, i); // Place pawns on 2nd rank
        }
        whiteRooks = setBit(whiteRooks, 0);     // A1
        whiteRooks = setBit(whiteRooks, 7);     // H1
        whiteKnights = setBit(whiteKnights, 1); // B1
        whiteKnights = setBit(whiteKnights, 6); // G1
        whiteBishops = setBit(whiteBishops, 2); // C1
        whiteBishops = setBit(whiteBishops, 5); // F1
        whiteQueens = setBit(whiteQueens, 3);   // D1
        whiteKing = setBit(whiteKing, 4);       // E1

        // Initialize black pieces
        for (int i = 48; i < 56; ++i)
        {
            blackPawns = setBit(blackPawns, i); // Place pawns on 7th rank
        }
        blackRooks = setBit(blackRooks, 56);     // A8
        blackRooks = setBit(blackRooks, 63);     // H8
        blackKnights = setBit(blackKnights, 57); // B8
        blackKnights = setBit(blackKnights, 62); // G8
        blackBishops = setBit(blackBishops, 58); // C8
        blackBishops = setBit(blackBishops, 61); // F8
        blackQueens = setBit(blackQueens, 59);   // D8
        blackKing = setBit(blackKing, 60);       // E8
    }

    // Copy constructor
    Position::Position(const Position &other) : whitePawns(other.whitePawns), whiteKnights(other.whiteKnights),
                                                whiteBishops(other.whiteBishops), whiteRooks(other.whiteRooks),
                                                whiteQueens(other.whiteQueens), whiteKing(other.whiteKing),
                                                blackPawns(other.blackPawns), blackKnights(other.blackKnights),
                                                blackBishops(other.blackBishops), blackRooks(other.blackRooks),
                                                blackQueens(other.blackQueens), blackKing(other.blackKing),
                                                castlingRights(other.castlingRights), enPassantSquare(other.enPassantSquare) {}

    Position::Position(const Position &other, const Move &move) : whitePawns(other.whitePawns), whiteKnights(other.whiteKnights),
                                                                  whiteBishops(other.whiteBishops), whiteRooks(other.whiteRooks),
                                                                  whiteQueens(other.whiteQueens), whiteKing(other.whiteKing),
                                                                  blackPawns(other.blackPawns), blackKnights(other.blackKnights),
                                                                  blackBishops(other.blackBishops), blackRooks(other.blackRooks),
                                                                  blackQueens(other.blackQueens), blackKing(other.blackKing),
                                                                  castlingRights(other.castlingRights), enPassantSquare(other.enPassantSquare)
    {
        makeMove(move);
    }

    // Getters for on the fly composite bitboards
    uint64_t Position::getWhitePieces() const { return whitePawns | whiteKnights | whiteBishops | whiteRooks | whiteQueens | whiteKing; }
    uint64_t Position::getBlackPieces() const { return blackPawns | blackKnights | blackBishops | blackRooks | blackQueens | blackKing; }
    uint64_t Position::getOccupiedSquares() const { return getWhitePieces() | getBlackPieces(); }
    uint64_t Position::getEmptySquares() const { return ~getOccupiedSquares(); }

    void Position::makeMove(const Move &move)
    {
        uint64_t fromBB = 1ULL << move.fromSquare;
        uint64_t toBB = 1ULL << move.toSquare;
        bool isWhite = (move.color == Color::WHITE);

        // Handle captures
        if (move.isCapture)
        {
            uint64_t captureBB = toBB;
            if (isWhite)
            {
                if (blackPawns & captureBB)
                    handleCapture(blackPawns, captureBB);
                else if (blackKnights & captureBB)
                    handleCapture(blackKnights, captureBB);
                else if (blackBishops & captureBB)
                    handleCapture(blackBishops, captureBB);
                else if (blackRooks & captureBB)
                    handleCapture(blackRooks, captureBB);
                else if (blackQueens & captureBB)
                    handleCapture(blackQueens, captureBB);
                else if (blackKing & captureBB)
                    handleCapture(blackKing, captureBB);
            }
            else
            {
                if (whitePawns & captureBB)
                    handleCapture(whitePawns, captureBB);
                else if (whiteKnights & captureBB)
                    handleCapture(whiteKnights, captureBB);
                else if (whiteBishops & captureBB)
                    handleCapture(whiteBishops, captureBB);
                else if (whiteRooks & captureBB)
                    handleCapture(whiteRooks, captureBB);
                else if (whiteQueens & captureBB)
                    handleCapture(whiteQueens, captureBB);
                else if (whiteKing & captureBB)
                    handleCapture(whiteKing, captureBB);
            }
        }

        // Remove piece from its original square and place it in the new one
        if (isWhite)
        {
            if (whitePawns & fromBB)
                updateBitboard(whitePawns, fromBB, toBB);
            else if (whiteKnights & fromBB)
                updateBitboard(whiteKnights, fromBB, toBB);
            else if (whiteBishops & fromBB)
                updateBitboard(whiteBishops, fromBB, toBB);
            else if (whiteRooks & fromBB)
                updateBitboard(whiteRooks, fromBB, toBB);
            else if (whiteQueens & fromBB)
                updateBitboard(whiteQueens, fromBB, toBB);
            else if (whiteKing & fromBB)
                updateBitboard(whiteKing, fromBB, toBB);
        }
        else
        {
            if (blackPawns & fromBB)
                updateBitboard(blackPawns, fromBB, toBB);
            else if (blackKnights & fromBB)
                updateBitboard(blackKnights, fromBB, toBB);
            else if (blackBishops & fromBB)
                updateBitboard(blackBishops, fromBB, toBB);
            else if (blackRooks & fromBB)
                updateBitboard(blackRooks, fromBB, toBB);
            else if (blackQueens & fromBB)
                updateBitboard(blackQueens, fromBB, toBB);
            else if (blackKing & fromBB)
                updateBitboard(blackKing, fromBB, toBB);
        }

        // Handle castling (move the rook)
        if (move.isCastling)
        {
            if (move.castlingType == CastlingType::KINGSIDE)
            {
                if (isWhite)
                {
                    whiteRooks &= ~(1ULL << 7);
                    whiteRooks |= (1ULL << 5);
                }
                else
                {
                    blackRooks &= ~(1ULL << 63);
                    blackRooks |= (1ULL << 61);
                }
            }
            else
            { // Queenside
                if (isWhite)
                {
                    whiteRooks &= ~(1ULL << 0);
                    whiteRooks |= (1ULL << 3);
                }
                else
                {
                    blackRooks &= ~(1ULL << 56);
                    blackRooks |= (1ULL << 59);
                }
            }
        }

        // Handle En Passant
        if (move.toSquare == move.prevEnPassantSquare)
        {
            if (isWhite)
            {
                blackPawns &= ~(1ULL << (move.toSquare - 8));
            }
            else
            {
                whitePawns &= ~(1ULL << (move.toSquare + 8));
            }
        }

        // Handle Promotion
        if (move.isPromotion)
        {
            if (isWhite)
            {
                whiteQueens |= toBB;
                whitePawns &= ~toBB;
            }
            else
            {
                blackQueens |= toBB;
                blackPawns &= ~toBB;
            }
        }
    }

    void Position::undoMove(const Move &move)
    {
        uint64_t fromBB = 1ULL << move.fromSquare;
        uint64_t toBB = 1ULL << move.toSquare;
        bool isWhite = (move.color == Color::WHITE);

        // Move piece back to its original square
        if (isWhite)
        {
            if (whitePawns & toBB)
                updateBitboard(whitePawns, toBB, fromBB);
            else if (whiteKnights & toBB)
                updateBitboard(whiteKnights, toBB, fromBB);
            else if (whiteBishops & toBB)
                updateBitboard(whiteBishops, toBB, fromBB);
            else if (whiteRooks & toBB)
                updateBitboard(whiteRooks, toBB, fromBB);
            else if (whiteQueens & toBB)
                updateBitboard(whiteQueens, toBB, fromBB);
            else if (whiteKing & toBB)
                updateBitboard(whiteKing, toBB, fromBB);
        }
        else
        {
            if (blackPawns & toBB)
                updateBitboard(blackPawns, toBB, fromBB);
            else if (blackKnights & toBB)
                updateBitboard(blackKnights, toBB, fromBB);
            else if (blackBishops & toBB)
                updateBitboard(blackBishops, toBB, fromBB);
            else if (blackRooks & toBB)
                updateBitboard(blackRooks, toBB, fromBB);
            else if (blackQueens & toBB)
                updateBitboard(blackQueens, toBB, fromBB);
            else if (blackKing & toBB)
                updateBitboard(blackKing, toBB, fromBB);
        }

        // Restore captured piece
        if (move.isCapture)
        {
            uint64_t captureBB = toBB;
            if (isWhite)
            {
                if (blackPawns & captureBB)
                    blackPawns |= captureBB;
                else if (blackKnights & captureBB)
                    blackKnights |= captureBB;
                else if (blackBishops & captureBB)
                    blackBishops |= captureBB;
                else if (blackRooks & captureBB)
                    blackRooks |= captureBB;
                else if (blackQueens & captureBB)
                    blackQueens |= captureBB;
                else if (blackKing & captureBB)
                    blackKing |= captureBB;
            }
            else
            {
                if (whitePawns & captureBB)
                    whitePawns |= captureBB;
                else if (whiteKnights & captureBB)
                    whiteKnights |= captureBB;
                else if (whiteBishops & captureBB)
                    whiteBishops |= captureBB;
                else if (whiteRooks & captureBB)
                    whiteRooks |= captureBB;
                else if (whiteQueens & captureBB)
                    whiteQueens |= captureBB;
                else if (whiteKing & captureBB)
                    whiteKing |= captureBB;
            }
        }

        // Undo Castling
        if (move.isCastling)
        {
            if (move.castlingType == CastlingType::KINGSIDE)
            {
                if (isWhite)
                {
                    whiteRooks |= (1ULL << 7);
                    whiteRooks &= ~(1ULL << 5);
                }
                else
                {
                    blackRooks |= (1ULL << 63);
                    blackRooks &= ~(1ULL << 61);
                }
            }
            else
            { // Queenside
                if (isWhite)
                {
                    whiteRooks |= (1ULL << 0);
                    whiteRooks &= ~(1ULL << 3);
                }
                else
                {
                    blackRooks |= (1ULL << 56);
                    blackRooks &= ~(1ULL << 59);
                }
            }
        }

        // Undo En Passant
        if (move.fromSquare == move.prevEnPassantSquare)
        {
            if (isWhite)
            {
                blackPawns |= (1ULL << (move.toSquare - 8));
            }
            else
            {
                whitePawns |= (1ULL << (move.toSquare + 8));
            }
        }

        // Undo Promotion
        if (move.isPromotion)
        {
            if (isWhite)
            {
                whiteQueens &= ~toBB;
                whitePawns |= fromBB;
            }
            else
            {
                blackQueens &= ~toBB;
                blackPawns |= fromBB;
            }
        }
    }

    // Computes the Zobrist hash of the position
    uint64_t Position::computeHash() const
    {
        uint64_t hash = 0;

        auto hashPieces = [&](uint64_t bitboard, int pieceIndex)
        {
            while (bitboard)
            {
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
        if (enPassantSquare != -1)
        {
            int file = enPassantSquare % 8;
            hash ^= zobristEnPassant[file];
        }

        // Castling rights hash
        hash ^= zobristCastling[castlingRights];

        return hash;
    }

    // helper functions
    // Displays the current chess board state in a human-readable format
    // Uses Unicode chess pieces and coordinate system (a-h, 1-8)
    std::string Position::displayPosition()
    {
        std::string returnable = "";
        // Loop through ranks from top (8) to bottom (1)
        for (int rank = 7; rank >= 0; --rank)
        {
            // Loop through files from left (a) to right (h)
            for (int file = 0; file < 8; ++file)
            {
                int square = rank * 8 + file; // Convert rank/file to square index
                if (getBit(whitePawns, square))
                    returnable.append("♙ "); // White pawn
                else if (getBit(whiteKnights, square))
                    returnable.append("♘ "); // White knight
                else if (getBit(whiteBishops, square))
                    returnable.append("♗ "); // White bishop
                else if (getBit(whiteRooks, square))
                    returnable.append("♖ "); // White rook
                else if (getBit(whiteQueens, square))
                    returnable.append("♕ "); // White queen
                else if (getBit(whiteKing, square))
                    returnable.append("♔ "); // White king
                else if (getBit(blackPawns, square))
                    returnable.append("♟ "); // Black pawn
                else if (getBit(blackKnights, square))
                    returnable.append("♞ "); // Black knight
                else if (getBit(blackBishops, square))
                    returnable.append("♝ "); // Black bishop
                else if (getBit(blackRooks, square))
                    returnable.append("♜ "); // Black rook
                else if (getBit(blackQueens, square))
                    returnable.append("♛ "); // Black queen
                else if (getBit(blackKing, square))
                    returnable.append("♚ "); // Black king
                else
                    returnable.append(". "); // Empty square
            }
            returnable.append("\n");
        }
        return returnable;
    }

    std::string Position::getFen(Color toMove, int halfmoveClock, int fullmoveNumber, std::string castlingRights, std::string enPassantTarget)
    {
        std::ostringstream fen;
        // Encode board position
        for (int rank = 7; rank >= 0; --rank)
        {
            int emptyCount = 0;
            for (int file = 0; file < 8; ++file)
            {
                int square = rank * 8 + file;
                char squareValue = getSquareChar(square);
                if (squareValue == '.')
                {
                    ++emptyCount;
                }
                else
                {
                    if (emptyCount > 0)
                    {
                        fen << emptyCount;
                        emptyCount = 0;
                    }
                    fen << squareValue;
                }
            }
            if (emptyCount > 0)
            {
                fen << emptyCount;
            }
            if (rank > 0)
            {
                fen << '/';
            }
        }

        // Active color
        fen << ' ' << colorToChar(toMove);

        // Castling availability
        fen << ' ' << (castlingRights.empty() ? "-" : castlingRights);

        // En passant target
        fen << ' ' << (enPassantTarget.empty() ? "-" : enPassantTarget);

        // Halfmove clock and fullmove number
        fen << ' ' << halfmoveClock << ' ' << fullmoveNumber;

        return fen.str();
    }
}
