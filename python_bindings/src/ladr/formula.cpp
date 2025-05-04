#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <string>
#include "term.hpp"
#include "formula.hpp"

// C headers for LADR
extern "C" {
    #include "../../../ladr/header.h"
    #include "../../../ladr/formula.h"
}

namespace py = pybind11;
// Helper function to convert a formula to a string
std::string formula_to_string_cpp(Formula f) {
    if (!f) return "NULL";
    
    std::string result;
    
    switch (f->type) {
        case ATOM_FORM:
            result = term_to_string_cpp(f->atom);
            break;
            
        case AND_FORM:
            result = "(" + formula_to_string_cpp(f->kids[0]) + " & " + 
                    formula_to_string_cpp(f->kids[1]) + ")";
            break;
            
        case OR_FORM:
            result = "(" + formula_to_string_cpp(f->kids[0]) + " | " +
                    formula_to_string_cpp(f->kids[1]) + ")";
            break;
            
        case NOT_FORM:
            result = "-" + formula_to_string_cpp(f->kids[0]);
            break;
            
        case IFF_FORM:
            result = "(" + formula_to_string_cpp(f->kids[0]) + " <-> " +
                    formula_to_string_cpp(f->kids[1]) + ")";
            break;
            
        case IMP_FORM:
            result = "(" + formula_to_string_cpp(f->kids[0]) + " -> " +
                    formula_to_string_cpp(f->kids[1]) + ")";
            break;
            
        case IMPBY_FORM:
            result = "(" + formula_to_string_cpp(f->kids[0]) + " <- " +
                    formula_to_string_cpp(f->kids[1]) + ")";
            break;
            
        case ALL_FORM:
            result = "all " + std::string(f->qvar) + " " + 
                    formula_to_string_cpp(f->kids[0]);
            break;
            
        case EXISTS_FORM:
            result = "exists " + std::string(f->qvar) + " " +
                    formula_to_string_cpp(f->kids[0]);
            break;
    }
    
    return result;
}

// Function to initialize the formula module
void init_formula_module(py::module_& m) {
    // Define the Formula type enum
    py::enum_<FormulaType>(m, "FormulaType")
        .value("ATOM", FormulaType::ATOM)
        .value("AND", FormulaType::AND)
        .value("OR", FormulaType::OR)
        .value("NOT", FormulaType::NOT)
        .value("IFF", FormulaType::IFF)
        .value("IMP", FormulaType::IMP)
        .value("IMPBY", FormulaType::IMPBY)
        .value("ALL", FormulaType::ALL)
        .value("EXISTS", FormulaType::EXISTS)
        .export_values();
    
    // Define the Formula class
    py::class_<struct formula, PyFormula>(m, "Formula")
        .def_property_readonly("type", [](const Formula& f) {
            return static_cast<FormulaType>(f->type);
        }, "Get the type of the formula")
        .def_property_readonly("arity", [](const Formula& f) {
            return f->arity;
        }, "Get the arity of the formula")
        .def_property_readonly("qvar", [](const Formula& f) {
            if (f->qvar)
                return std::string(f->qvar);
            return std::string("");
        }, "Get the quantified variable (for quantified formulas)")
        .def_property_readonly("atom", [](const Formula& f) {
            if (f->type == ATOM_FORM && f->atom)
                return copy_term(f->atom);
            throw py::value_error("Formula is not an atom");
        }, "Get the atom term (for atom formulas)")
        .def("__getitem__", [](const Formula& f, int i) {
            if (f->type == ATOM_FORM)
                throw py::type_error("Atom formulas don't have subformulas");
            if (i < 0 || i >= f->arity)
                throw py::index_error("Formula subformula index out of range");
            return formula_copy(f->kids[i]);
        }, "Get a subformula")
        .def("__len__", [](const Formula& f) {
            return f->arity;
        }, "Get the number of subformulas")
        .def("__str__", [](const Formula& f) {
            return formula_to_string_cpp(f);
        }, "Convert the formula to a string")
        .def("__repr__", [](const Formula& f) {
            return "Formula(\"" + formula_to_string_cpp(f) + "\")";
        }, "Formula representation");
    
    // Conversion between Term and Formula
    m.def("term_to_formula", [](Term term) {
        return PyFormula(term_to_formula(term));
    }, "Convert a term to a formula", py::arg("term"));
    
    m.def("formula_to_term", [](Formula f) {
        return PyTerm(formula_to_term(f));
    }, "Convert a formula to a term", py::arg("f"));
    
    // Formula construction functions
    m.def("formula_copy", [](Formula f) {
        return PyFormula(formula_copy(f));
    }, "Create a copy of a formula", py::arg("f"));
    
    m.def("negate", [](Formula f) {
        return PyFormula(negate(f));
    }, "Negate a formula", py::arg("f"));
    
    m.def("and", [](Formula a, Formula b) {
        return PyFormula(f_and(a, b));
    }, "Create a conjunction of two formulas", py::arg("a"), py::arg("b"));
    
    m.def("or", [](Formula a, Formula b) {
        return PyFormula(f_or(a, b));
    }, "Create a disjunction of two formulas", py::arg("a"), py::arg("b"));
    
    m.def("imp", [](Formula a, Formula b) {
        return PyFormula(imp(a, b));
    }, "Create an implication formula", py::arg("a"), py::arg("b"));
    
    m.def("impby", [](Formula a, Formula b) {
        return PyFormula(impby(a, b));
    }, "Create a reverse implication formula", py::arg("a"), py::arg("b"));
    
    m.def("nnf", [](Formula f) {
        return PyFormula(nnf(f));
    }, "Convert a formula to negation normal form", py::arg("f"));
    
    m.def("flatten", [](Formula f) {
        return PyFormula(formula_flatten(f));
    }, "Flatten nested conjunctions and disjunctions", py::arg("f"));
    
    m.def("universal_closure", [](Formula f) {
        return PyFormula(universal_closure(f));
    }, "Create universal closure of a formula", py::arg("f"));
    
    m.def("size", [](Formula f) {
        return formula_size(f);
    }, "Calculate the size of a formula", py::arg("f"));
    
    m.def("hash", [](Formula f) {
        return hash_formula(f);
    }, "Calculate a hash value for a formula", py::arg("f"));
    
    m.def("ident", [](Formula f, Formula g) {
        return formula_ident(f, g) == TRUE;
    }, "Check if two formulas are identical", py::arg("f"), py::arg("g"));
    
    // Predicates
    m.def("literal", [](Formula f) {
        return literal_formula(f) == TRUE;
    }, "Check if a formula is a literal", py::arg("f"));
    
    m.def("clausal", [](Formula f) {
        return clausal_formula(f) == TRUE;
    }, "Check if a formula is clausal", py::arg("f"));
    
    m.def("positive", [](Formula f) {
        return positive_formula(f) == TRUE;
    }, "Check if a formula is positive", py::arg("f"));
    
    m.def("closed", [](Formula f) {
        return closed_formula(f) == TRUE;
    }, "Check if a formula is closed (no free variables)", py::arg("f"));
}

// Module initialization
PYBIND11_MODULE(formula, m) {
    init_formula_module(m);
}