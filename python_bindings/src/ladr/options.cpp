
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <string>

// C headers for LADR
extern "C" {
    #include "../../../ladr/header.h"
    #include "../../../ladr/options.h"
}

namespace py = pybind11;

// Function to initialize the options module
void init_options_module(py::module_& m) {
    
    // Make a binding for the flag struct


    m.def("str_to_flag_id", [](std::string& flag_name) {
        return str_to_flag_id((char*)flag_name.c_str());
    }, "Get flag id from flag name", py::arg("flag_name"));

    m.def("set_flag", [](int flag_id, bool value) {
        set_flag(flag_id, value ? TRUE : FALSE);
    }, "Set flag value", py::arg("flag_id"), py::arg("value"));
}