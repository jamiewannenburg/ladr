#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <string>
#include "../common/error_handling.hpp"
#include "glist.hpp"

// Ensure C linkage for all the LADR headers
extern "C" {
    #include "../../../ladr/glist.h"
    #include "../../../ladr/term.h"
    // plist.h is included in term.h, no need for separate include
    // #include "../../../ladr/plist.h"
}

namespace py = pybind11;
using namespace ladr;

namespace pybind11 { namespace detail {
    // Specialization for Term*
    template <>
    class plist_caster<Term> {
    public:
        static py::list to_python_list(Plist lst) {
            py::list result;
            Plist p;
            for (p = lst; p != NULL; p = p->next) {
                // Add each term to the list
                // We need to wrap it in a smart pointer to handle memory management
                Term t = static_cast<Term>(p->v);
                if (t) {
                    // Create a copy of the term to avoid memory issues
                    // The original term might be freed elsewhere
                    Term t_copy = copy_term(t);
                    result.append(t_copy);
                }
            }
            return result;
        }
    };
}}

// Function to initialize the glist module
void init_glist_module(py::module_& m) {
    // Register error handling
    register_error_handling(m);
    
    // Functions for Ilist
    m.def("ilist_append", [](Ilist lst, int i) {
        reset_error_flag();
        auto result = ilist_append(lst, i);
        check_for_errors();
        return result;
    }, "Append an integer to an Ilist", py::arg("lst"), py::arg("i"));

    m.def("ilist_prepend", [](Ilist lst, int i) {
        reset_error_flag();
        auto result = ilist_prepend(lst, i);
        check_for_errors();
        return result;
    }, "Prepend an integer to an Ilist", py::arg("lst"), py::arg("i"));

    m.def("ilist_to_python_list", [](Ilist lst) {
        reset_error_flag();
        std::vector<int> result;
        Ilist p;
        for (p = lst; p != NULL; p = p->next) {
            result.push_back(p->i);
        }
        check_for_errors();
        return result;
    }, "Convert an Ilist to a Python list of integers", py::arg("lst"));

    // Plist conversion functions
    m.def("plist_to_python_list", [](Plist lst) {
        reset_error_flag();
        auto result = pybind11::detail::plist_caster<Term>::to_python_list(lst);
        check_for_errors();
        return result;
    }, "Convert a Plist to a Python list of Terms", py::arg("lst"));
}

// For backward compatibility with direct module import
PYBIND11_MODULE(glist, m) {
    m.doc() = "Python bindings for LADR glist module";
    init_glist_module(m);
} 