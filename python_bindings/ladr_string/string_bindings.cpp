#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "string.h"

namespace py = pybind11;

PYBIND11_MODULE(string_bindings, m) {
    m.doc() = "Python bindings for LADR string module"; // optional module docstring

    m.def("str_ident", &str_ident, "Check if two strings are identical",
          py::arg("s"), py::arg("t"));

    m.def("initial_substring", &initial_substring, "Check if x is an initial substring of y",
          py::arg("x"), py::arg("y"));

    m.def("substring", &substring, "Check if x is a substring of y",
          py::arg("x"), py::arg("y"));

    m.def("reverse_chars", &reverse_chars, "Reverse characters in a string between start and end positions",
          py::arg("s"), py::arg("start"), py::arg("end"));

    m.def("natural_string", &natural_string, "Convert string to natural number (-1 if not possible)",
          py::arg("str"));

    m.def("char_occurrences", &char_occurrences, "Count occurrences of a character in a string",
          py::arg("s"), py::arg("c"));

    m.def("str_to_int", [](const std::string& str) {
        int result;
        if (str_to_int(str.c_str(), &result)) {
            return py::cast(result);
        }
        throw py::value_error("Could not convert string to integer");
    }, "Convert string to integer");

    m.def("str_to_double", [](const std::string& str) {
        double result;
        if (str_to_double(str.c_str(), &result)) {
            return py::cast(result);
        }
        throw py::value_error("Could not convert string to double");
    }, "Convert string to double");

    m.def("string_of_repeated", &string_of_repeated, "Check if string consists of repeated character",
          py::arg("c"), py::arg("s"));
} 