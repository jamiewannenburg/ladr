#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "glist.hpp"
#include "common/conversions.hpp"

// Ensure C linkage for all the LADR headers
extern "C" {
    #include "../../../ladr/glist.h"
    #include "../../../ladr/term.h"
}

namespace py = pybind11;

PYBIND11_MODULE(glist, m) {
    m.doc() = "Python bindings for LADR glist module";

    // Functions for Ilist
    m.def("ilist_append", [](Ilist lst, int i) {
        return ilist_append(lst, i);
    }, "Append an integer to an Ilist", py::arg("lst"), py::arg("i"));

    m.def("ilist_prepend", [](Ilist lst, int i) {
        return ilist_prepend(lst, i);
    }, "Prepend an integer to an Ilist", py::arg("lst"), py::arg("i"));

    m.def("ilist_count", &ilist_count, "Count elements in an Ilist", py::arg("lst"));

    m.def("ilist_member", [](Ilist lst, int i) {
        return ilist_member(lst, i) == TRUE;
    }, "Check if an integer is in an Ilist", py::arg("lst"), py::arg("i"));

    m.def("zap_ilist", &zap_ilist, "Free an entire Ilist", py::arg("lst"));

    // Functions for Plist
    m.def("plist_count", &plist_count, "Count elements in a Plist", py::arg("lst"));

    m.def("plist_member", [](Plist lst, void* v) {
        return plist_member(lst, v) == TRUE;
    }, "Check if a pointer is in a Plist", py::arg("lst"), py::arg("v"));

    m.def("zap_plist", &zap_plist, "Free an entire Plist", py::arg("lst"));

    // Register the Term-specific Plist functions for better type safety
    m.def("plist_of_terms", [](py::list terms) {
        // Convert Python list of Terms to C Plist
        return pybind11::detail::plist_caster<Term>::from_python_list(terms);
    }, "Create a Plist from a Python list of Terms", py::arg("terms"));

    m.def("terms_from_plist", [](Plist lst) {
        // Convert C Plist to Python list of Terms
        py::list result = pybind11::detail::plist_caster<Term>::to_python_list(lst);
        return result;
    }, "Convert a Plist to a Python list of Terms", py::arg("lst"));
} 