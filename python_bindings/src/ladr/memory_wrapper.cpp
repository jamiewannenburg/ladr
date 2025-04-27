#include <pybind11/pybind11.h>
#include <string>
#include "../common/error_handling.hpp"

namespace py = pybind11;
using namespace ladr;

PYBIND11_MODULE(memory, m) {
    m.doc() = "Python bindings for LADR memory module (wrapper)";
    
    // Register error handling
    register_error_handling(m);
    
    try {
        // Import the combined module
        py::module combined = py::module::import("ladr_combined.memory");
        
        // Re-export all attributes that aren't special Python names
        for (const auto& item : combined.attr("__dir__")()) {
            std::string name = py::cast<std::string>(item);
            if (!name.empty() && name[0] != '_') {
                m.attr(name.c_str()) = combined.attr(name.c_str());
            }
        }
    } catch (py::error_already_set& e) {
        // If import fails, provide a more helpful error message
        std::string err = std::string("Failed to import ladr_combined.memory module: ") + e.what();
        m.attr("import_error") = err;
        
        // Define a function to report the error
        m.def("get_import_error", []() {
            throw py::import_error("Failed to import ladr_combined.memory module. Make sure it's properly installed.");
        });
    }
} 