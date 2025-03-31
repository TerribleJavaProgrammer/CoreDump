#include <pybind11/pybind11.h>
#include "console.h"

namespace py = pybind11;
namespace cd = coredump;

PYBIND11_MODULE(core_dump_py, handle)
{
	handle.def("test_fn", [](float a, float b)
			   { return a + b; });

	// Bind the Color enum to Python
	py::enum_<cd::Color>(handle, "Color")
		.value("WHITE", cd::Color::WHITE)
		.value("BLACK", cd::Color::BLACK);

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
		.def_readonly("fromSquare", &cd::Move::fromSquare)
		.def_readonly("toSquare", &cd::Move::toSquare)
		.def_readonly("isCapture", &cd::Move::isCapture)
		.def_readonly("isCastling", &cd::Move::isCastling)
		.def_readonly("pieceType", &cd::Move::pieceType)
		.def_readonly("color", &cd::Move::color)
		.def_readonly("castlingType", &cd::Move::castlingType)
		.def_readonly("isPromotion", &cd::Move::isPromotion)
		.def_readonly("promotionPiece", &cd::Move::promotionPiece)
		.doc() = "A chess move. From, to, isCapture, isCastling, pieceType, color, castlingType, isPromotion, promotionPiece";

	// Bind the Position class to Python, including constructors and methods
	py::class_<cd::Position>(handle, "Position")
		.def(py::init<>())
		.def(py::init<const cd::Position &>())
		.def(py::init<const cd::Position &, const cd::Move &>())
		.def("makeMove", &cd::Position::makeMove)
		.def("undoMove", &cd::Position::undoMove)
		.def("displayPosition", &cd::Position::displayPosition)
		.def("getFen", &cd::Position::getFen);

	handle.def("findBestMove", &cd::findBestMove);
}

int main()
{
	std::cout << "==========STARTING ENGINE==========" << std::endl;
	return cd::start_console();
}
