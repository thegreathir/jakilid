#include <pybind11/pybind11.h>
#include "jakilid.hpp"

namespace py = pybind11;

PYBIND11_MODULE(jakilid, m) {
    m.doc() = "pybind11 example plugin"; // optional module docstring

    m.def("add", [](int x) -> py::object {
        if (x == 0) {
            return py::int_(17);
        }

        return py::str("ggg");

    }, "A function which adds two numbers");

}
