#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <string>
#include "term.hpp"

// C headers for LADR
extern "C" {
    #include "../../../ladr/header.h"
    #include "../../../ladr/formula.h"
}

namespace py = pybind11;

// Function to initialize the formula module
void init_formula_module(py::module_& m) {

    m.def("term_to_formula", [](Term term) {
        return term_to_formula(term);
    }, "Convert a term to a formula", py::arg("term"));

}