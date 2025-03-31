#include "console.h"

#define MAX_DEPTH 2
#define MAX_TIME 5
#define DEBUG true
#define USE_HUMAN true

namespace coredump
{
    // 0 is valid, 1 is invalid move, -1 is quit
    int makeHumanTurn(const Position &currentPosition, Color currentPlayer, Move &move)
    {
        std::cout << "\nCommands:" << std::endl;
        std::cout << "- Make move: e2 e4" << std::endl;
        // std::cout << "- Show moves for square: moves e2" << std::endl;
        std::cout << "- Quit: quit> " << std::endl;
        std::string input = "";
        std::getline(std::cin, input);

        if (input == "quit")
        {
            std::cout << "Quitting the game" << std::endl;
            return -1;
        }
        // Handle move command
        if (input.length() != 5 || input[2] != ' ')
        {
            std::cout << "Invalid move format. Use 'e2 e4'\n";
            return 1;
        }

        // Create move to check legality
        move.fromSquare = Move::fromAlgebraic(input[0], input[1]);
        move.toSquare = Move::fromAlgebraic(input[3], input[4]);
        move.color = currentPlayer;

        std::vector<Move> legalMoves = generateMoves(currentPosition, currentPlayer);
        auto moveIt = std::find_if(legalMoves.begin(), legalMoves.end(),
                                   [&move](const Move &m)
                                   {
                                       return m.fromSquare == move.fromSquare && m.toSquare == move.toSquare;
                                   });

        if (moveIt == legalMoves.end())
        {
            std::cout << "Illegal move." << std::endl;
            return 1;
        }

        move = *moveIt; // Copy extra properties from legal move

        // Handle pawn promotion
        if (move.isPromotion)
        {
            std::cout << "Choose promotion piece Q(ueen), R(ook), B(ishop), k(N)ight: ";
            char choice;
            std::cin >> choice;
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

            move.promotionPiece = getPromotionPiece(choice);
            if (move.promotionPiece == PieceType::NONE)
            {
                std::cout << "Invalid promotion piece. Defaulting to Queen.\n";
                move.promotionPiece = PieceType::QUEEN;
            }
        }

        return 0; // Valid move
    }

    int start_console()
    {
        // Initialize the position
        Position currentPosition;

        // TODO make this run more automatically for when in the pybind module
        initializeMagicBitboards();

        Color currentPlayer = Color::WHITE; // White moves first
        Color humanColor = Color::WHITE;    // Human plays white by default
        int fullmoveCounter = 0;
        int halfmoveClock = 0;
        std::ostringstream pgn;

        // Let player choose color
        std::cout << "Play as (w)hite or (b)lack? ";
        char choice;
        std::cin >> choice;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        if (tolower(choice) == 'q')
        {
            std::cout << "Quitting the game" << std::endl;
            return 0;
        }
        if (tolower(choice) == 'b')
        {
            humanColor = Color::BLACK;
        }

        // print out who plays who
        std::cout << "You are playing as " << colorToString(humanColor) << std::endl;
        std::cout << "AI is playing as " << colorToString(invertColor(humanColor)) << std::endl;
        if (humanColor == Color::WHITE)
        {
            std::cout << "You play first" << std::endl;
        }
        else
        {
            std::cout << "AI plays first" << std::endl;
        }

        // Main game loop
        while (true)
        {
            // Display the current position
            std::cout << "\n====================" << std::endl;
            std::cout << "Move " << fullmoveCounter << std::endl;

            std::cout << colorToString(currentPlayer) << " to move\n";

            int status = checkEndgameConditions(currentPosition, currentPlayer);
            if (status == 1)
            {
                std::cout << "CHECK!\n";
            }
            else if (status == 2)
            {
                std::cout << "CHECKMATE! " << colorToString(invertColor(currentPlayer)) << " wins." << std::endl;
                break;
            }
            else if (status == 3)
            {
                std::cout << "STALEMATE! Nobody wins" << std::endl;
                break;
            }

            // TODO add more endgame conditions
            // else if (isInsufficientMaterial(currentPosition)) {
            // 	std::cout << "INSUFFICIENT MATERIAL!\n";
            // 	break;
            // }
            // else if (isThreefoldRepetition(currentPosition)) {
            // 	std::cout << "THREEFOLD REPETITION!\n";
            // 	break;
            // }
            // else if (isFiftyMoveRule(currentPosition)) {
            // 	std::cout << "FIFTY MOVE RULE!\n";
            // 	break;
            // }

            std::cout << currentPosition.displayPosition() << std::endl;
            std::cout << currentPosition.getFen(currentPlayer, halfmoveClock, fullmoveCounter, "", "") << std::endl;
            std::cout << "PGN:\n";
            std::cout << pgn.str() << std::endl;

            // Determine if it's human's turn
            bool isHumanTurn = (currentPlayer == humanColor) && USE_HUMAN;
            Move move;
            if (isHumanTurn)
            {
                bool quit = false;
                while (true)
                {
                    int status = makeHumanTurn(currentPosition, currentPlayer, move);
                    if (status == 0)
                    {
                        break; // Move was made successfully
                    }
                    else if (status == -1)
                    {
                        quit = true;
                        break;
                    }
                }
                if (quit)
                {
                    break;
                }
            }
            else
            {
                // AI's turn
                std::cout << "AI is thinking...\n";
                std::vector<Move> legalMoves = generateMoves(currentPosition, currentPlayer);

                std::cout << "Legal moves: " << legalMoves.size() << std::endl;
                // print all the legal moves in algebraic notation
                for (const auto &m : legalMoves)
                {
                    Position tempPos(currentPosition);
                    tempPos.makeMove(m);
                    int evaluation = evaluatePosition(tempPos, currentPlayer);
                    std::cout << Move::toAlgebraic(m.fromSquare) << " " << Move::toAlgebraic(m.toSquare) << " scores " << evaluation << std::endl;
                }

                move = findBestMove(currentPosition, currentPlayer, MAX_DEPTH, MAX_TIME, DEBUG);
                // Convert move to algebraic notation for display
                std::string moveStr = Move::toAlgebraic(move.fromSquare) + " " + Move::toAlgebraic(move.toSquare);
                std::cout << "Computer plays: " << moveStr << std::endl;
            }

            // Make the selected move
            currentPosition.makeMove(move);

            // update pgn
            if (currentPlayer == Color::WHITE)
            {
                fullmoveCounter += 1;
                pgn << fullmoveCounter;
                pgn << '.';
            }
            pgn << move.getPgn();
            pgn << ' ';

            // Switch to next player
            currentPlayer = invertColor(currentPlayer);
        }

        std::cout << "\nGame ended.\n";
        std::cout << "PGN:\n";
        std::cout << pgn.str() << std::endl;
        return 0;
    }
}