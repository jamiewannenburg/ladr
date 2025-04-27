#include <pybind11/pybind11.h>
#include "../common/error_handling.hpp"

// Use the common error handling utilities
namespace py = pybind11;
using namespace ladr;

// Function to initialize the error handler module
void init_error_handler_module(py::module_& m) {
    // Register all error handling functions using our common utility
    register_error_handling(m);
}

// For backward compatibility with direct module import
PYBIND11_MODULE(error_handler, m) {
    m.doc() = "Python bindings for LADR error handler module";
    init_error_handler_module(m);
} 