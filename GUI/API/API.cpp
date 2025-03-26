#include <sstream>

#include "board/board.h"
#include "board/magic/magicbitboard.h"
#include "board/position.h"
#include "engine-related/engine.h"
#include "move/movegen.h"

// for interaction with Python (tkinter lib)

int main() {
    initializeMagicBitboards();
    std::string command;
    std::string pColorStr;
    bool pIsWhite = true;

    while (true) {
        std::getline(std::cin, command);
        std::istringstream iss(command);
        std::string action;
        iss >> action;

        if (action == "init") {
            iss >> pColorStr;
            pIsWhite = (pColorStr == "white");
            currPosition.initializePosition();
            if (!pIsWhite) {
                makeMove(currPosition, findBestMove(Move::Color::WHITE, 50, 3, false));
            }
            std::cout << "initSuccess" << std::endl;
        } else if (action == "move") {
            int fromSquare, toSquare;
            iss >> fromSquare >> toSquare;
            Move move;
            move.fromSquare = fromSquare;
            move.toSquare = toSquare;
            move.color = pIsWhite ? Move::Color::WHITE : Move::Color::BLACK;
            std::vector<Move> legalMoves = generateMoves(currPosition.getPosition(), move.color);
            auto moveIt = std::find_if(legalMoves.begin(), legalMoves.end(),
                [&move](const Move& m) {
                    return m.fromSquare == move.fromSquare && m.toSquare == move.toSquare;
                });

            if (moveIt == legalMoves.end()) {
                std::cout << "invalidMove" << std::endl;
            }
            else {
                move = *moveIt;
                if (move.isPromotion) {
                    move.promotionPiece = Move::PieceType::QUEEN;
                }
                makeMove(currPosition, move);
                std::cout << "moveMade" << std::endl;
            }
        } else if (action == "fetchState") {
            std::string state = currPosition.displayPosition();
            std::string returnable;
            for (int i = 0; i < state.size(); i++) {
                if (state.substr(i, 1) != " ") {
                    returnable += state[i];
                }
            }
            std::cout << returnable << std::endl;
        } else if (action == "botMove") {
            std::cerr << "recieved" << std::endl; // NOT RECIEVED
            std::cout.flush(); // NOT RECIEVED
            makeMove(currPosition, findBestMove((pIsWhite ? Move::Color::BLACK : Move::Color::WHITE), 50, 3, false));
            std::cout << "botMoved" << std::endl; // NOT RECIEVED
        } else if (action == "isMate") {
            bool noMovesWhite = generateMoves(currPosition, Move::Color::WHITE).size() == 0;
            bool noMovesBlack = generateMoves(currPosition, Move::Color::BLACK).size() == 0;
            bool isInCheckWhite = isInCheck(currPosition, Move::Color::WHITE);
            bool isInCheckBlack = isInCheck(currPosition, Move::Color::BLACK);
            if (noMovesWhite || noMovesBlack) {
                if ((noMovesWhite && isInCheckWhite) || (noMovesBlack && isInCheckBlack)) {
                    std::cout << (noMovesWhite ? "blackWins" : "whiteWins") << std::endl;
                } else {
                    std::cout << "staleMate" << std::endl;
                }
            }
        } else if (action == "getMoves"){
            int square;
            iss >> square;
            std::vector<Move> relevantMoves = generateMoves(currPosition, (pIsWhite ? Move::Color::WHITE : Move::Color::BLACK));
            relevantMoves.erase(std::remove_if(relevantMoves.begin(), relevantMoves.end(), [square](const Move& move){
                return move.fromSquare != square;
            }), relevantMoves.end());
            std::cout << "moves";
            for (const auto& move : relevantMoves) {
                std::cout << " " << move.toSquare;
            }
            std::cout << std::endl;
        } else if (action == "quit") {
            break;
        }
    }

    return 0;
}