#include <pybind11/pybind11.h>
#include "../../ladr/fatal.h"
#include <iostream>

namespace py = pybind11;

// Custom wrapper for fatal_error that raises a Python FatalError
void py_fatal_error(const char* message) {
    // Import our custom FatalError exception
    py::module errors = py::module::import("ladr_bindings.errors");
    py::object FatalError = errors.attr("FatalError");
    
    // Get the current exit code
    int exit_code = get_fatal_exit_code();
    
    // Create the exception object
    py::object exception = FatalError(message, exit_code);
    
    // Set the Python exception
    PyErr_SetObject(exception.get_type().ptr(), exception.ptr());
    
    // This will be caught by pybind11 and converted to a Python exception
    throw py::error_already_set();
}

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

    // Bind our custom fatal error function instead of the original one
    m.def("fatal_error", &py_fatal_error,
          "Raise a FatalError with the given error message",
          py::arg("message"));
} 