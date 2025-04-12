#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <string>
#include <cstdio>
#include <cstdlib>
#include <stdexcept> // For std::runtime_error
#include <memory>

// Include LADR headers for types and functions
extern "C" {
    #include "../../ladr/header.h"
    #include "../../ladr/term.h"
}

namespace py = pybind11;

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

    // Define the formula struct partially - we include the fields we need
    struct formula {
        Ftype       type;
        int         arity;
        char        *qvar;         /* quantified variable */
        Formula     *kids;         /* for non-atoms */
        Term        atom;          /* for atoms */
        struct attribute *attributes;
        int         excess_refs;   /* count of extra references */
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
    
    // Term <-> Formula conversion
    Formula term_to_formula(Term t);
    Term formula_to_term(Formula f);
    
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
        .def_property_readonly("atom", [](const Formula& f) {
            if (f->type == ATOM_FORM) {
                return copy_term(f->atom);
            } else {
                throw py::value_error("Non-atomic formulas don't have an atom");
            }
        }, "Get the atom term for atomic formulas")
        .def_property_readonly("qvar", [](const Formula& f) {
            if (quant_form(f) == TRUE) {
                return std::string(f->qvar);
            } else {
                throw py::value_error("Non-quantified formulas don't have a qvar");
            }
        }, "Get the quantified variable for quantified formulas")
        .def("__getitem__", [](const Formula& f, int i) {
            if (f->type == ATOM_FORM) {
                throw py::value_error("Atomic formulas don't have subformulas");
            }
            if (i < 0 || i >= f->arity) {
                throw py::index_error("Formula subformula index out of range");
            }
            
            // Return a copy of the subformula to avoid ownership issues
            if (f->kids[i] && f->kids[i]->type == ATOM_FORM && f->kids[i]->atom) {
                // Special handling for atomic subformulas
                Term atom_copy = copy_term(f->kids[i]->atom);
                Formula result = formula_get(0, ATOM_FORM);
                result->atom = atom_copy;
                return result;
            }
            else {
                // Regular case
                return formula_copy(f->kids[i]);
            }
        }, "Get a subformula of a non-atomic formula")
        .def("__len__", [](const Formula& f) {
            return f->arity;
        }, "Get the number of subformulas")
        .def("__str__", [](const Formula& f) {
            // Special handling for atomic formulas
            if (f->type == ATOM_FORM) {
                if (f->atom) {
                    char* str = term_to_string(f->atom);
                    std::string result(str);
                    free(str);
                    return result;
                } else {
                    return std::string("(empty atom)");
                }
            }
            // Special handling for conjunctions/disjunctions
            else if ((f->type == AND_FORM || f->type == OR_FORM) && f->arity == 2) {
                std::string op = (f->type == AND_FORM) ? " & " : " | ";
                std::string left = "";
                std::string right = "";
                
                // Handle left side
                if (f->kids[0]) {
                    if (f->kids[0]->type == ATOM_FORM && f->kids[0]->atom) {
                        char* str = term_to_string(f->kids[0]->atom);
                        left = std::string(str);
                        free(str);
                    } else {
                        char* tmp = NULL;
                        FILE* tmpf = tmpfile();
                        if (tmpf) {
                            fprint_formula(tmpf, f->kids[0]);
                            fflush(tmpf);
                            fseek(tmpf, 0, SEEK_END);
                            size_t size = ftell(tmpf);
                            rewind(tmpf);
                            if (size > 0) {
                                tmp = (char*)malloc(size + 1);
                                fread(tmp, 1, size, tmpf);
                                tmp[size] = '\0';
                                left = std::string(tmp);
                                free(tmp);
                            }
                            fclose(tmpf);
                        }
                    }
                }
                
                // Handle right side
                if (f->kids[1]) {
                    if (f->kids[1]->type == ATOM_FORM && f->kids[1]->atom) {
                        char* str = term_to_string(f->kids[1]->atom);
                        right = std::string(str);
                        free(str);
                    } else {
                        char* tmp = NULL;
                        FILE* tmpf = tmpfile();
                        if (tmpf) {
                            fprint_formula(tmpf, f->kids[1]);
                            fflush(tmpf);
                            fseek(tmpf, 0, SEEK_END);
                            size_t size = ftell(tmpf);
                            rewind(tmpf);
                            if (size > 0) {
                                tmp = (char*)malloc(size + 1);
                                fread(tmp, 1, size, tmpf);
                                tmp[size] = '\0';
                                right = std::string(tmp);
                                free(tmp);
                            }
                            fclose(tmpf);
                        }
                    }
                }
                
                return "(" + left + op + right + ")";
            }
            // Default handling for other formula types
            else {
                return formula_to_string_cpp(f);
            }
        }, "Convert the formula to a string")
        .def("__repr__", [](const Formula& f) {
            if (f->type == ATOM_FORM && f->atom) {
                // For atomic formulas, just print the atom
                char* str = term_to_string(f->atom);
                std::string result = std::string(str);
                free(str);
                return result;
            }
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
        // Must create deep copies to avoid losing the atom values
        Formula a_copy = formula_copy(a);
        Formula b_copy = formula_copy(b);
        
        // Create the AND formula
        Formula result = formula_get(2, AND_FORM);
        
        // Set the kids directly
        result->kids[0] = a_copy;
        result->kids[1] = b_copy;
        
        return result;
    }, "Create a conjunction of two formulas", py::arg("a"), py::arg("b"));

    m.def("or_form", [](Formula a, Formula b) {
        // Must create deep copies to avoid losing the atom values
        Formula a_copy = formula_copy(a);
        Formula b_copy = formula_copy(b);
        
        // Create the OR formula
        Formula result = formula_get(2, OR_FORM);
        
        // Set the kids directly
        result->kids[0] = a_copy;
        result->kids[1] = b_copy;
        
        return result;
    }, "Create a disjunction of two formulas", py::arg("a"), py::arg("b"));

    m.def("imp_form", [](Formula a, Formula b) {
        // Must create deep copies to avoid losing the atom values
        Formula a_copy = formula_copy(a);
        Formula b_copy = formula_copy(b);
        
        // Create the IMP formula
        Formula result = formula_get(2, IMP_FORM);
        
        // Set the kids directly
        result->kids[0] = a_copy;
        result->kids[1] = b_copy;
        
        return result;
    }, "Create an implication formula", py::arg("a"), py::arg("b"));

    m.def("impby_form", [](Formula a, Formula b) {
        // Must create deep copies to avoid losing the atom values
        Formula a_copy = formula_copy(a);
        Formula b_copy = formula_copy(b);
        
        // Create the IMPBY formula
        Formula result = formula_get(2, IMPBY_FORM);
        
        // Set the kids directly
        result->kids[0] = a_copy;
        result->kids[1] = b_copy;
        
        return result;
    }, "Create a reverse implication formula", py::arg("a"), py::arg("b"));

    m.def("negate", [](Formula a) {
        // Must create a deep copy to avoid losing the atom values
        Formula a_copy = formula_copy(a);
        
        // Create the NOT formula
        Formula result = formula_get(1, NOT_FORM);
        
        // Set the kid directly
        result->kids[0] = a_copy;
        
        return result;
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

    // Term <-> Formula conversion
    m.def("term_to_formula", [](Term t) {
        return term_to_formula(copy_term(t));
    }, "Convert a term to a formula", py::arg("t"));

    m.def("formula_to_term", [](Formula f) {
        return formula_to_term(formula_copy(f));
    }, "Convert a formula to a term", py::arg("f"));
    
    // Debug function to help diagnose formula issues
    m.def("debug_formula", [](Formula f) {
        std::string result = "Formula debug:\n";
        result += "  Type: " + std::to_string(f->type) + "\n";
        result += "  Arity: " + std::to_string(f->arity) + "\n";
        
        if (f->type == ATOM_FORM && f->atom) {
            char* atom_str = term_to_string(f->atom);
            result += "  Atom: " + std::string(atom_str) + "\n";
            free(atom_str);
        }
        else if (f->arity > 0 && f->kids) {
            result += "  Subformulas:\n";
            for (int i = 0; i < f->arity; i++) {
                if (f->kids[i]) {
                    char* tmp = NULL;
                    if (f->kids[i]->type == ATOM_FORM && f->kids[i]->atom) {
                        tmp = term_to_string(f->kids[i]->atom);
                        result += "    [" + std::to_string(i) + "]: ATOM = " + std::string(tmp) + "\n";
                    }
                    else {
                        result += "    [" + std::to_string(i) + "]: (complex formula of type " + 
                                  std::to_string(f->kids[i]->type) + ")\n";
                    }
                    if (tmp) free(tmp);
                }
                else {
                    result += "    [" + std::to_string(i) + "]: NULL\n";
                }
            }
        }
        
        return result;
    }, "Debug info about a formula", py::arg("f"));

    // Formula utilities
    m.def("formula_to_string", [](Formula f) {
        if (f->type == ATOM_FORM && f->atom) {
            char* str = term_to_string(f->atom);
            std::string result(str);
            free(str);
            return result;
        } else {
            return formula_to_string_cpp(f);
        }
    }, "Convert a formula to a string representation", py::arg("f"));
}
