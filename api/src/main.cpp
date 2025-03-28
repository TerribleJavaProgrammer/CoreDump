#include "main.h"

namespace py = pybind11;

PYBIND11_MODULE(core_dump, handle)
{
	handle.def("test_fn", [](float a, float b)
			   { return a + b });

	// Bind the Color enum to Python
	py::enum_<Color>(handle, "Color")
		.value("WHITE", Color::WHITE)
		.value("BLACK", Color::BLACK);

	// Bind the PieceType enum to Python
	py::enum_<PieceType>(handle, "PieceType")
		.value("PAWN", PieceType::PAWN)
		.value("KNIGHT", PieceType::KNIGHT)
		.value("BISHOP", PieceType::BISHOP)
		.value("ROOK", PieceType::ROOK)
		.value("QUEEN", PieceType::QUEEN)
		.value("KING", PieceType::KING)
		.value("NONE", PieceType::NONE);

	// Bind the Move class to Python, including constructors and methods
	py::class_<Move>(handle, "Move")
		.def(py::init<int, int, bool, Color, bool>())
		.def_readonly("fromSquare", &Move::fromSquare)
		.def_readonly("toSquare", &Move::toSquare)
		.def_readonly("isCapture", &Move::isCapture)
		.def_readonly("isCastling", &Move::isCastling)
		.def_readonly("pieceType", &Move::pieceType)
		.def_readonly("color", &Move::color)
		.def_readonly("castlingType", &Move::castlingType)
		.def_readonly("isPromotion", &Move::isPromotion)
		.def_readonly("promotionPiece", &Move::promotionPiece)
		.doc() = "A chess move. From, to, isCapture, isCastling, pieceType, color, castlingType, isPromotion, promotionPiece";

	// Bind the Position class to Python, including constructors and methods
	py::class_<Position>(handle, "Position")
		.def(py::init<>())
		.def(py::init<const Position &>())
		.def(py::init<const Position &, const Move &>())
		.def("makeMove", &Position::makeMove)
		.def("undoMove", &Position::undoMove)
		.def("displayPosition", &Position::displayPosition)
		.def("getFen", &Position::getFen);

	handle.def("findBestMove", &findBestMove);
}
