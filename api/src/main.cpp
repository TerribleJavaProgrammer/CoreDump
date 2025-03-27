#include <vector>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "board/board.h"

namespace py = pybind11;

float test_fn(float arg1, float arg2)
{
	return arg1 + arg2;
}

PYBIND11_MODULE(core_dump, handle)
{
	handle.def("test_fn", [](float arg1, float arg2)
			   { return test_fn(arg1, arg2); });

	py::class_<Board>(handle, "ChessBoard")
		.def(py::init<float>())
		.def("multiply", &SomeClass::multiply)
		.def("multiply_list", &SomeClass::multiply_list);
}
