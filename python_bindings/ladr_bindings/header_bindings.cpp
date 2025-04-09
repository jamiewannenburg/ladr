#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "../../ladr/header.h"

namespace py = pybind11;

PYBIND11_MODULE(header, m) {
    m.doc() = "Python bindings for LADR header module"; 

    // Bind the BOOL enum
    py::enum_<BOOL>(m, "BOOL")
        .value("FALSE", FALSE)
        .value("TRUE", TRUE)
        .export_values();

    // Bind the IMAX and IMIN macros as functions
    m.def("imax", [](int a, int b) { return IMAX(a, b); }, "Get maximum of two integers");
    m.def("imin", [](int a, int b) { return IMIN(a, b); }, "Get minimum of two integers");

    // Bind DBL_LARGE as a constant
    m.attr("DBL_LARGE") = DBL_LARGE;
} 