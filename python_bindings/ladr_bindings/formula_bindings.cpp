#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <string>
#include <cstdio>
#include <cstdlib>
#include <stdexcept> // For std::runtime_error
#include <memory>

namespace py = pybind11;

// Define BOOL type
typedef int BOOL;
#define TRUE 1
#define FALSE 0

// Forward declare the structs we need
struct formula;

// Define the typedef for the types
typedef struct formula *Formula;

// Forward declare the LADR functions we need
extern "C" {
    // Formula types
    typedef enum {
        ATOM_FORM=0, AND_FORM, OR_FORM, NOT_FORM, IFF_FORM,
        IMP_FORM, IMPBY_FORM, ALL_FORM, EXISTS_FORM
    } Ftype;

    // Define the formula struct partially - we only access type and arity
    struct formula {
        Ftype       type;
        int         arity;
        // Other fields omitted for simplicity
    };

    // Memory management
    Formula formula_get(int arity, Ftype type);
    void zap_formula(Formula f);

    // Formula operations
    int formula_size(Formula f);
    BOOL quant_form(Formula f);
    BOOL closed_formula(Formula f);
    BOOL literal_formula(Formula f);
    BOOL clausal_formula(Formula f);
    BOOL formula_ident(Formula f, Formula g);
    Formula formula_copy(Formula f);
    Formula negate(Formula a);
    Formula imp(Formula a, Formula b);
    Formula impby(Formula a, Formula b);
    Formula flatten_top(Formula f);
    Formula formula_flatten(Formula f);
    Formula nnf(Formula f);
    Formula universal_closure(Formula f);
    
    // Output
    void fprint_formula(FILE *fp, Formula f);
    
    // Special operations that are problematic in C++
    Formula formula_and(Formula a, Formula b);
    Formula formula_or(Formula a, Formula b);
}

// Define TRUE/FALSE formula macros
inline int TRUE_FORMULA(Formula f) {
    return f->type == AND_FORM && f->arity == 0;
}

inline int FALSE_FORMULA(Formula f) {
    return f->type == OR_FORM && f->arity == 0;
}

// Custom string conversion function
std::string formula_to_string_cpp(Formula f) {
    // Create a temporary file to write the formula to
    FILE* tmp = tmpfile();
    if (!tmp) {
        throw std::runtime_error("Failed to create temporary file");
    }
    
    // Write the formula to the temporary file
    fprint_formula(tmp, f);
    std::fflush(tmp);
    
    // Get file size
    std::fseek(tmp, 0, SEEK_END);
    size_t size = std::ftell(tmp);
    std::rewind(tmp);
    
    // Read the file into a string
    std::string result;
    result.resize(size);
    if (size > 0) {
        std::fread(&result[0], 1, size, tmp);
    }
    
    // Close the file
    std::fclose(tmp);
    
    // Remove trailing newline if present
    if (!result.empty() && result.back() == '\n') {
        result.pop_back();
    }
    
    return result;
}

// Custom deleter for Formula objects
struct FormulaDeleter {
    void operator()(Formula f) const {
        if (f) zap_formula(f);
    }
};

using PyFormula = std::unique_ptr<struct formula, FormulaDeleter>;

PYBIND11_MODULE(formula, m) {
    m.doc() = "Python bindings for LADR formula module";

    // Define the Ftype enum
    py::enum_<Ftype>(m, "Ftype")
        .value("ATOM_FORM", ATOM_FORM)
        .value("AND_FORM", AND_FORM)
        .value("OR_FORM", OR_FORM)
        .value("NOT_FORM", NOT_FORM)
        .value("IFF_FORM", IFF_FORM)
        .value("IMP_FORM", IMP_FORM)
        .value("IMPBY_FORM", IMPBY_FORM)
        .value("ALL_FORM", ALL_FORM)
        .value("EXISTS_FORM", EXISTS_FORM)
        .export_values();

    // Define the Formula class with minimal functionality
    py::class_<struct formula, PyFormula>(m, "Formula")
        .def_property_readonly("type", [](const Formula& f) {
            return f->type;
        }, "Get the formula type")
        .def_property_readonly("arity", [](const Formula& f) {
            return f->arity;
        }, "Get the arity of the formula")
        .def_property_readonly("is_atom", [](const Formula& f) {
            return f->type == ATOM_FORM;
        }, "Check if the formula is an atom")
        .def_property_readonly("is_quantified", [](const Formula& f) {
            return quant_form(f) == TRUE;
        }, "Check if the formula is quantified")
        .def("__str__", [](const Formula& f) {
            return formula_to_string_cpp(f);
        }, "Convert the formula to a string")
        .def("__repr__", [](const Formula& f) {
            return "Formula(\"" + formula_to_string_cpp(f) + "\")";
        }, "Formula representation")
        .def("__bool__", [](const Formula& f) {
            return f != nullptr;
        });

    // Create formulas of different types
    m.def("formula_get", [](int arity, Ftype type) {
        return formula_get(arity, type);
    }, "Create a formula of the given type and arity", 
       py::arg("arity"), py::arg("type"));

    // Formula operations
    m.def("formula_copy", &formula_copy, "Create a copy of a formula", py::arg("f"));

    m.def("formula_ident", [](Formula f1, Formula f2) {
        return formula_ident(f1, f2) == TRUE;
    }, "Check if two formulas are identical", py::arg("f1"), py::arg("f2"));

    // Use the functions with C++ safe names
    m.def("and_form", [](Formula a, Formula b) {
        return formula_and(formula_copy(a), formula_copy(b));
    }, "Create a conjunction of two formulas", py::arg("a"), py::arg("b"));

    m.def("or_form", [](Formula a, Formula b) {
        return formula_or(formula_copy(a), formula_copy(b));
    }, "Create a disjunction of two formulas", py::arg("a"), py::arg("b"));

    m.def("imp_form", [](Formula a, Formula b) {
        return imp(formula_copy(a), formula_copy(b));
    }, "Create an implication formula", py::arg("a"), py::arg("b"));

    m.def("impby_form", [](Formula a, Formula b) {
        return impby(formula_copy(a), formula_copy(b));
    }, "Create a reverse implication formula", py::arg("a"), py::arg("b"));

    m.def("negate", [](Formula a) {
        return negate(formula_copy(a));
    }, "Create the negation of a formula", py::arg("a"));

    m.def("quant_form", [](Formula f) {
        return quant_form(f) == TRUE;
    }, "Check if a formula is quantified", py::arg("f"));

    m.def("universal_closure", [](Formula f) {
        return universal_closure(formula_copy(f));
    }, "Get the universal closure of a formula", py::arg("f"));

    m.def("flatten_top", [](Formula f) {
        return flatten_top(formula_copy(f));
    }, "Flatten the top level of a formula", py::arg("f"));

    m.def("formula_flatten", [](Formula f) {
        return formula_flatten(formula_copy(f));
    }, "Flatten a formula by removing nested AND/OR", py::arg("f"));

    m.def("nnf", [](Formula f) {
        return nnf(formula_copy(f));
    }, "Convert a formula to negation normal form", py::arg("f"));

    m.def("literal_formula", [](Formula f) {
        return literal_formula(f) == TRUE;
    }, "Check if a formula is a literal", py::arg("f"));

    m.def("clausal_formula", [](Formula f) {
        return clausal_formula(f) == TRUE;
    }, "Check if a formula is in clausal form", py::arg("f"));

    m.def("formula_size", [](Formula f) {
        return formula_size(f);
    }, "Get the size of a formula", py::arg("f"));

    m.def("closed_formula", [](Formula f) {
        return closed_formula(f) == TRUE;
    }, "Check if a formula has no free variables", py::arg("f"));

    // Special constant formulas
    m.def("true_formula", [](Formula f) {
        return TRUE_FORMULA(f);
    }, "Check if a formula is TRUE", py::arg("f"));

    m.def("false_formula", [](Formula f) {
        return FALSE_FORMULA(f);
    }, "Check if a formula is FALSE", py::arg("f"));

    // Create TRUE/FALSE formulas
    m.def("get_true_formula", []() {
        return formula_get(0, AND_FORM);  // Empty conjunction is TRUE
    }, "Create a TRUE formula");

    m.def("get_false_formula", []() {
        return formula_get(0, OR_FORM);   // Empty disjunction is FALSE
    }, "Create a FALSE formula");
}
