#include <pybind11/pybind11.h>
#include "jakilid.hpp"


PYBIND11_MODULE(jakilid, m) {
    m.doc() = "pybind11 example plugin"; // optional module docstring

    // m.def("add", &add, "A function which adds two numbers");
}
