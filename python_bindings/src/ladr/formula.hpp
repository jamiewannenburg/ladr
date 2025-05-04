#ifndef LADR_FORMULA_HPP
#define LADR_FORMULA_HPP
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <string>
#include "term.hpp"

extern "C" {
    #include "../../../ladr/formula.h"
}

std::string formula_to_string_cpp(Formula f);

namespace py = pybind11;

// Custom deleter for Formula objects
struct FormulaDeleter {
    void operator()(Formula f) const {
        if (f == NULL) return;
        
        try {
            zap_formula(f);
        } catch (...) {
            // Last resort - if anything goes wrong, try free_formula
            try {
                free_formula(f);
            } catch (...) {
                // If all else fails, just leak the memory rather than crash
            }
        }
    }
};

using PyFormula = std::unique_ptr<struct formula, FormulaDeleter>;

// Enum for formula types
enum class FormulaType {
    ATOM = ATOM_FORM,
    AND = AND_FORM,
    OR = OR_FORM,
    NOT = NOT_FORM,
    IFF = IFF_FORM,
    IMP = IMP_FORM,
    IMPBY = IMPBY_FORM,
    ALL = ALL_FORM,
    EXISTS = EXISTS_FORM
};

// Forward declaration for init function to be called from module init
void init_formula_module(py::module_& m);

#endif // LADR_FORMULA_HPP 