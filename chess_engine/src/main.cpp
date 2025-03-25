#include "board/board.h"
#include "board/magic/magicbitboard.h"
#include "board/position.h"
#include "engine-related/engine.h"
#include "move/movegen.h"

#include <iostream>
#include <stdexcept>
#include <string>
#include <sstream>

int main() {
    // Initialize the game state
    currPosition.initializePosition();        // Set up initial piece positions
    initializeMagicBitboards();   // Pre-calculate move lookup tables
    bool gameOver = false;
    Move::Color currentPlayer = Move::Color::WHITE;  // White moves first
    bool playerIsWhite = true;    // Default: human plays white
    int fullmoveCounter = 0;
    std::ostringstream pgn;
    
    // Let player choose their color
    std::cout << "Play as (w)hite or (b)lack? ";
    char choice;
    std::cin >> choice;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    playerIsWhite = (tolower(choice) != 'b');

    // Main game loop
    while (!gameOver) {
        // Display current game state
        std::cout << "\n====================\n";
        std::cout << (currentPlayer == Move::Color::WHITE ? "White" : "Black") << " to move\n";
        if (isInCheck(currPosition, currentPlayer)) {
            std::cout << "CHECK!\n";
        }
        
        bool currentPlayerIsWhite = currentPlayer == Move::Color::WHITE;
        std::cout << currPosition.displayPosition() << std::endl;
        std::cout << currPosition.getFen(currentPlayerIsWhite, 0, fullmoveCounter, "", "") << std::endl;

        // Determine if it's player's turn
        bool isPlayerTurn = currentPlayerIsWhite == playerIsWhite;
        isPlayerTurn = isPlayerTurn && false;
        Move move;

        if (isPlayerTurn) {
            // Human player's turn
            std::cout << "\nCommands:\n";
            std::cout << "- Move: e2 e4\n";
            std::cout << "- Show moves: moves e2\n";
            std::cout << "- Quit: quit\n> ";
            
            std::string input;
            std::getline(std::cin, input);

            // Handle quit command
            if (input == "quit") {
                break;
            }

            // Handle move command
            if (input.length() != 5 || input[2] != ' ') {
                std::cout << "Invalid move format. Use 'e2 e4'\n";
                continue;
            }

            // Create and validate move
            move.fromSquare = Move::fromAlgebraic(input[0], input[1]);
            move.toSquare = Move::fromAlgebraic(input[3], input[4]);
            move.color = currentPlayer;

            // Check if move is legal
            std::vector<Move> legalMoves = generateMoves(currPosition.getPosition(), currentPlayer);
            auto moveIt = std::find_if(legalMoves.begin(), legalMoves.end(),
                [&move](const Move& m) {
                    return m.fromSquare == move.fromSquare && m.toSquare == move.toSquare;
                });

            if (moveIt == legalMoves.end()) {
                std::cout << "Illegal move." << legalMoves.size() << '\n';
                continue;
            }

            // Copy all properties from legal move
            move = *moveIt;

            // Handle pawn promotion
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

            makeMove(currPosition, move);
        } else {
            // Computer's turn
            std::cout << "Computer is thinking...\n";
            auto start = std::chrono::high_resolution_clock::now();
            move = findBestMove(currentPlayer, 5, 100); // Search n ply deep
            auto end = std::chrono::high_resolution_clock::now();
            double timeTaken = std::chrono::duration<double>(end - start).count();
            std::cout << "(Time taken: " << timeTaken << "s)\n";
            // Convert move to algebraic notation for display
            std::string moveStr = 
                std::string(1, 'a' + (move.fromSquare % 8)) +
                std::string(1, '1' + (move.fromSquare / 8)) + " " +
                std::string(1, 'a' + (move.toSquare % 8)) +
                std::string(1, '1' + (move.toSquare / 8));
                
            std::cout << "Computer plays: " << moveStr << std::endl;
            
            makeMove(currPosition, move);
        }

        // Check for game end conditions
        std::vector<Move> nextMoves = generateMoves(currPosition.getPosition(), currentPlayer);
        if (nextMoves.empty()) {
            if (isInCheck(currPosition.getPosition(), currentPlayer)) {
                std::cout << "\nCheckmate! " 
                         << (currentPlayer == Move::Color::WHITE ? "Black" : "White") 
                         << " wins!\n";
            } else {
                std::cout << "\nStalemate! Game is a draw.\n";
            }
            gameOver = true;
            continue;
        }

        // update pgn
        if (currentPlayerIsWhite) {
            fullmoveCounter += 1;
            pgn << fullmoveCounter;
            pgn << '.';
        }
        pgn << move.getPgn();
        pgn << ' ';

        // Switch to next player
        currentPlayer = (currentPlayer == Move::Color::WHITE) ? 
            Move::Color::BLACK : Move::Color::WHITE;
    }

    std::cout << "\nGame ended.\n";
    std::cout << "PGN:\n";
    std::cout << pgn.str() << std::endl;
    return 0;
}
