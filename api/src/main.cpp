// #include <pybind11/pybind11.h>
#include "engine-related/engine.h"
#include <iostream>
#include <sstream>
#include <string>

// namespace py = pybind11;
namespace cd = coredump;

#define MAX_DEPTH 7
#define MAX_TIME 200
#define DEBUG true
#define USE_HUMAN true

namespace coredump
{
	PieceType getPromotionPiece(char piece)
	{
		switch (piece)
		{
		case 'Q':
			return PieceType::QUEEN;
		case 'N':
			return PieceType::KNIGHT;
		case 'B':
			return PieceType::BISHOP;
		case 'R':
			return PieceType::ROOK;
		default:
			return PieceType::NONE;
		}
	}

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
			std::cout << "Illegal move." << legalMoves.size() << '\n';
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

	int sub_main()
	{
		// Initialize the position
		Position currentPosition;

		// TODO make this runa more automatically for when in the pybind module
		initializeMagicBitboards();

		Color currentPlayer = Color::WHITE; // White moves first
		Color humanColor = Color::WHITE;	// Human plays white by default
		int fullmoveCounter = 0;
		int halfmoveClock = 0;
		std::ostringstream pgn;

		// Let player choose color
		std::cout << "Play as (w)hite or (b)lack? ";
		char choice;
		std::cin >> choice;
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		if (tolower(choice) == 'b')
		{
			humanColor = Color::BLACK;
		}

		// Main game loop
		while (true)
		{
			// Display the current position
			std::cout << "\n====================" << std::endl;
			std::cout << "Move " << fullmoveCounter << std::endl;

			std::cout << colorToString(currentPlayer) << " to move\n";
			if (isInCheck(currentPosition, currentPlayer))
			{
				std::cout << "CHECK!\n";
			}

			// TODO add endgame conditions
			// if (isInCheckmate(currentPosition, currentPlayer)) {
			// 	std::cout << "CHECKMATE!\n";
			// 	break;
			// }
			// else if (isStalemate(currentPosition, currentPlayer)) {
			// 	std::cout << "STALEMATE!\n";
			// 	break;
			// }
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
				move = findBestMove(currentPosition, currentPlayer, MAX_DEPTH, MAX_TIME, DEBUG);
				// Convert move to algebraic notation for display
				std::string moveStr = Move::toAlgebraic(move.fromSquare) + " " + Move::toAlgebraic(move.toSquare);
				std::cout << "Computer plays: " << moveStr << std::endl;
			}

			// Make the selected move
			currentPosition.makeMove(move);

			// Check for game end conditions

			// Check if the other player is in check after this player's move
			std::vector<Move> nextMoves = generateMoves(currentPosition, invertColor(currentPlayer));
			if (nextMoves.empty())
			{
				if (isInCheck(currentPosition, currentPlayer))
				{
					std::cout << "\nCheckmate! "
							  << (currentPlayer == Color::WHITE ? "Black" : "White")
							  << " wins!\n";
				}
				else
				{
					std::cout << "\nStalemate! Game is a draw.\n";
				}
				break;
			}

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

int main()
{
	std::cout << "==========STARTING ENGINE==========" << std::endl;
	return cd::sub_main();
}

// PYBIND11_MODULE(core_dump, handle)
// {
// 	handle.def("test_fn", [](float a, float b)
// 			   { return a + b; });

// 	// Bind the Color enum to Python
// 	py::enum_<cd::Color>(handle, "Color")
// 		.value("WHITE", cd::Color::WHITE)
// 		.value("BLACK", cd::Color::BLACK);

// 	// Bind the PieceType enum to Python
// 	py::enum_<cd::PieceType>(handle, "PieceType")
// 		.value("PAWN", cd::PieceType::PAWN)
// 		.value("KNIGHT", cd::PieceType::KNIGHT)
// 		.value("BISHOP", cd::PieceType::BISHOP)
// 		.value("ROOK", cd::PieceType::ROOK)
// 		.value("QUEEN", cd::PieceType::QUEEN)
// 		.value("KING", cd::PieceType::KING)
// 		.value("NONE", cd::PieceType::NONE);

// 	// Bind the Move class to Python, including constructors and methods
// 	py::class_<cd::Move>(handle, "Move")
// 		.def(py::init<int, int, bool, cd::PieceType, cd::Color, bool>())
// 		.def_readonly("fromSquare", &cd::Move::fromSquare)
// 		.def_readonly("toSquare", &cd::Move::toSquare)
// 		.def_readonly("isCapture", &cd::Move::isCapture)
// 		.def_readonly("isCastling", &cd::Move::isCastling)
// 		.def_readonly("pieceType", &cd::Move::pieceType)
// 		.def_readonly("color", &cd::Move::color)
// 		.def_readonly("castlingType", &cd::Move::castlingType)
// 		.def_readonly("isPromotion", &cd::Move::isPromotion)
// 		.def_readonly("promotionPiece", &cd::Move::promotionPiece)
// 		.doc() = "A chess move. From, to, isCapture, isCastling, pieceType, color, castlingType, isPromotion, promotionPiece";

// 	// Bind the Position class to Python, including constructors and methods
// 	py::class_<cd::Position>(handle, "Position")
// 		.def(py::init<>())
// 		.def(py::init<const cd::Position &>())
// 		.def(py::init<const cd::Position &, const cd::Move &>())
// 		.def("makeMove", &cd::Position::makeMove)
// 		.def("undoMove", &cd::Position::undoMove)
// 		.def("displayPosition", &cd::Position::displayPosition)
// 		.def("getFen", &cd::Position::getFen);

// 	handle.def("findBestMove", &cd::findBestMove);
// }
