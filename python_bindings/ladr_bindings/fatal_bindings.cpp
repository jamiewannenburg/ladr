#include <pybind11/pybind11.h>
#include "../../ladr/fatal.h"
#include <iostream>

namespace py = pybind11;

PYBIND11_MODULE(fatal, m) {
    m.doc() = "Python bindings for LADR fatal error handling module";

    // Bind the bell function to write directly to Python's sys.stdout
    m.def("bell", []() {
        py::module sys = py::module::import("sys");
        py::object stdout = sys.attr("stdout");
        stdout.attr("write")("\007");
        stdout.attr("flush")();
    }, "Send the bell character to stdout");

    // Bind the fatal exit code functions
    m.def("get_fatal_exit_code", &get_fatal_exit_code,
          "Get the current fatal exit code");

    m.def("set_fatal_exit_code", &set_fatal_exit_code,
          "Set the fatal exit code",
          py::arg("exit_code"));

    // Bind the fatal error function
    m.def("fatal_error", &fatal_error,
          "Print a fatal error message and exit with the current fatal exit code",
          py::arg("message"));
} 