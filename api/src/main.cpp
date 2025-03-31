#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "console.h"

namespace py = pybind11;
namespace cd = coredump;

PYBIND11_MODULE(core_dump_py, handle)
{
	handle.doc() = "Core Dump Chess Engine Python Bindings";
	handle.attr("__version__") = "0.1.0";
	handle.attr("__author__") = "terriblejavaprogrammer, avidcoder27";
	handle.attr("__description__") = "A chess engine written in C++ with Python bindings using pybind11.";
	handle.attr("__license__") = "MIT";

	handle.def("engine_init", []()
			   { cd::initializeMagicBitboards(); });

	handle.def("find_best_move", [](const cd::Position &position, cd::Color color, int maxDepth, double timeLimitSeconds, bool debug)
			   {
		std::ostringstream debugStream;
		cd::Move move = cd::findBestMove(position, color, maxDepth, timeLimitSeconds, debug, debugStream);
		return py::make_tuple(move, debugStream.str()); });

	handle.def("find_random_move", &cd::findRandomMove);
	handle.def("generate_moves", &cd::generateMoves);
	handle.def("check_endgame_conditions", &cd::checkEndgameConditions);
	handle.def("invert_color", &cd::invertColor);

	handle.def("get_promotion_piece", [](const std::string &piece)
			   {
				   if (piece.length() != 1)
				   {
					   throw std::invalid_argument("Invalid piece length");
				   }
				   return cd::getPromotionPiece(piece[0]); });

	// Bind the Color enum to Python
	py::enum_<cd::Color>(handle, "Color")
		.value("WHITE", cd::Color::WHITE)
		.value("BLACK", cd::Color::BLACK)
		.def("to_string", &cd::colorToString);

	// Bind the PieceType enum to Python
	py::enum_<cd::PieceType>(handle, "PieceType")
		.value("PAWN", cd::PieceType::PAWN)
		.value("KNIGHT", cd::PieceType::KNIGHT)
		.value("BISHOP", cd::PieceType::BISHOP)
		.value("ROOK", cd::PieceType::ROOK)
		.value("QUEEN", cd::PieceType::QUEEN)
		.value("KING", cd::PieceType::KING)
		.value("NONE", cd::PieceType::NONE);

	// Bind the Move class to Python, including constructors and methods
	py::class_<cd::Move>(handle, "Move")
		.def(py::init<int, int, bool, cd::PieceType, cd::Color, bool>())
		.def(py::init<std::string, cd::Color>())
		.def("set_promotion_piece", [](cd::Move &move, cd::PieceType piece)
			 { move.promotionPiece = piece; })
		.def("get_pgn", &cd::Move::getPgn)
		.def_readonly("from_square", &cd::Move::fromSquare)
		.def_readonly("to_square", &cd::Move::toSquare)
		.def_readonly("is_capture", &cd::Move::isCapture)
		.def_readonly("is_castling", &cd::Move::isCastling)
		.def_readonly("piece_type", &cd::Move::pieceType)
		.def_readonly("color", &cd::Move::color)
		.def_readonly("castling_type", &cd::Move::castlingType)
		.def_readonly("is_promotion", &cd::Move::isPromotion)
		.def_readonly("promotion_piece", &cd::Move::promotionPiece)
		.def_static("to_algebraic", &cd::Move::toAlgebraic)
		.def_static("from_algebraic", &cd::Move::fromAlgebraic)
		.def("__eq__", &cd::Move::operator==)
		.doc() = "A chess move. From, to, isCapture, isCastling, pieceType, color, castlingType, isPromotion, promotionPiece";

	// Bind the Position class to Python, including constructors and methods
	py::class_<cd::Position>(handle, "Position")
		.def(py::init<>())
		.def(py::init<const cd::Position &>())
		.def(py::init<const cd::Position &, const cd::Move &>())
		.def("make_move", &cd::Position::makeMove)
		.def("undo_move", &cd::Position::undoMove)
		.def("display_position", &cd::Position::displayPosition)
		.def("get_fen", &cd::Position::getFen);
}

int main()
{
	std::cout << "==========STARTING ENGINE==========" << std::endl;
	return cd::start_console();
}
