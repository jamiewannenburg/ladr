#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <string>
#include "../common/error_handling.hpp"
#include "glist.hpp"
#include "common/conversions.hpp"

// Ensure C linkage for all the LADR headers
extern "C" {
    #include "../../../ladr/header.h" // Include header.h directly for BOOL enum
    #include "../../../ladr/term.h"
    #include "../../../ladr/top_input.h" // for init_standard_ladr
    #include "../../../ladr/symbols.h"
}

namespace py = pybind11;
using namespace ladr;

// Custom string conversion functions
std::string term_to_string_cpp(Term t) {
    char* str = term_to_string(t);
    std::string result(str);
    free(str);
    return result;
}

// External function to check if a term is a rigid term (needs free_term)
// or a regular term (needs zap_term)
bool is_rigid_term(Term t) {
    // Several ways to detect if a term needs free_term vs zap_term:
    // 1. A standalone rigid term created with get_rigid_term (args array uninitialized)
    // 2. If any arg is NULL but arity > 0, it's likely a rigid term with uninitialized args
    
    if (t == NULL) return false;
    
    // Handle constant terms separately (arity 0)
    if (t->arity == 0 && t->private_symbol < 0) return true;
    
    // For complex terms, only use free_term if the args array is not fully initialized
    if (t->private_symbol < 0 && t->arity > 0) {
        if (t->args == NULL) return true;
        
        // Check if any argument is NULL
        for (int i = 0; i < t->arity; i++) {
            if (t->args[i] == NULL) return true;
        }
    }
    
    // Default to false - use zap_term for fully initialized complex terms
    return false;
}

// Custom deleter for Term objects
struct TermDeleter {
    void operator()(Term t) const {
        if (t == NULL) return;
        
        try {
            // Extra safety check - don't try to free variables
            if (t->private_symbol >= 0) {
                // Variables don't need to be freed
                return;
            }
            
            bool is_rigid = is_rigid_term(t);
            if (is_rigid) {
                free_term(t);
            } else {
                // Carefully check each argument before zapping
                bool can_safely_zap = true;
                if (t->arity > 0 && t->args) {
                    for (int i = 0; i < t->arity; i++) {
                        if (t->args[i] == NULL) {
                            can_safely_zap = false;
                            break;
                        }
                    }
                }
                
                if (can_safely_zap) {
                    zap_term(t);
                } else {
                    // Fall back to free_term for any unsafe cases
                    free_term(t);
                }
            }
        } catch (...) {
            // Last resort - if anything goes wrong, try free_term
            try {
                free_term(t);
            } catch (...) {
                // If all else fails, just leak the memory rather than crash
                // std::cerr << "Warning: Failed to properly free term " << t << std::endl;
            }
        }
    }
};

using PyTerm = std::unique_ptr<struct term, TermDeleter>;

// Initialize the symbol table only once
static bool symbol_table_initialized = false;

void init_symbol_table() {
    if (!symbol_table_initialized) {
        // Initialize LADR packages including the symbol table
        init_standard_ladr();  // This will call declare_base_symbols and declare_standard_symbols
        
        symbol_table_initialized = true;
    }
}

// Function to initialize the term module
void init_term_module(py::module_& m) {
    // Register error handling
    register_error_handling(m);
    
    // Define constants
    m.attr("MAX_VARS") = MAX_VARS;
    m.attr("MAX_VNUM") = MAX_VNUM;
    m.attr("MAX_VAR") = MAX_VAR;
    m.attr("MAX_SYM") = MAX_SYM;
    m.attr("MAX_ARITY") = MAX_ARITY;

    // Define the BOOL enum that was previously in header.h
    py::enum_<BOOL>(m, "BOOL")
        .value("FALSE", FALSE)
        .value("TRUE", TRUE)
        .export_values();

    // Define the Term class
    py::class_<struct term, PyTerm>(m, "Term")
        .def_property_readonly("symbol", [](const Term& t) {
            if (VARIABLE(t)) {
                return std::string("v" + std::to_string(VARNUM(t)));
            } else {
                return std::string(sn_to_str(SYMNUM(t)));
            }
        }, "Get the symbol of the term")
        .def_property_readonly("arity", [](const Term& t) {
            return ARITY(t);
        }, "Get the arity of the term")
        .def_property_readonly("is_variable", [](const Term& t) {
            return VARIABLE(t) ? true : false;
        }, "Check if the term is a variable")
        .def_property_readonly("is_constant", [](const Term& t) {
            return CONSTANT(t) ? true : false;
        }, "Check if the term is a constant")
        .def_property_readonly("is_complex", [](const Term& t) {
            return COMPLEX(t) ? true : false;
        }, "Check if the term is a complex term")
        .def_property_readonly("symnum", [](const Term& t) {
            if (!VARIABLE(t)) {
                return SYMNUM(t);
            } else {
                throw py::value_error("Variable terms don't have a symnum");
            }
        }, "Get the symbol number (for non-variable terms)")
        .def_property_readonly("varnum", [](const Term& t) {
            if (VARIABLE(t)) {
                return VARNUM(t);
            } else {
                throw py::value_error("Non-variable terms don't have a varnum");
            }
        }, "Get the variable number (for variable terms)")
        .def("__getitem__", [](const Term& t, int i) {
            if (i < 0 || i >= ARITY(t)) {
                throw py::index_error("Term argument index out of range");
            }
            // Return a copy of the argument to avoid ownership issues
            return copy_term(ARG(t, i));
        }, "Get an argument of the term")
        .def("__len__", [](const Term& t) {
            return ARITY(t);
        }, "Get the number of arguments")
        .def("__str__", [](const Term& t) {
            return term_to_string_cpp(t);
        }, "Convert the term to a string")
        .def("__repr__", [](const Term& t) {
            return "Term(\"" + term_to_string_cpp(t) + "\")";
        }, "Term representation");

    // Add the set_of_variables function that returns a Plist
    m.def("set_of_variables", [](Term t) {
        reset_error_flag();
        
        // Call the C function
        Plist vars = set_of_variables(t);
        
        // Convert to a Python list using the conversion functions
        py::list result = pybind11::detail::plist_caster<Term>::to_python_list(vars);
        
        // Free the original Plist (but not its contents, as they're now referenced by Python)
        zap_plist(vars);
        
        check_for_errors();
        return result;
    }, "Get a list of all variables in a term", py::arg("t"));

    // Term creation functions
    m.def("get_variable_term", [](int var_num) {
        reset_error_flag();
        
        if (var_num < 0 || var_num > MAX_VAR) {
            throw py::value_error("Variable number out of range [0, MAX_VAR]");
        }
        
        auto result = PyTerm(get_variable_term(var_num));
        check_for_errors();
        return result;
    }, "Create a variable term", py::arg("var_num"));

    m.def("get_rigid_term", [](const std::string& sym, int arity) {
        reset_error_flag();
        
        if (arity < 0 || arity > MAX_ARITY) {
            throw py::value_error("Arity out of range [0, MAX_ARITY]");
        }
        // Get a rigid term (should be freed with free_term)
        Term t = get_rigid_term((char*)sym.c_str(), arity);
        if (!t) {
            throw py::value_error("Failed to create rigid term");
        }
        
        // Return a PyTerm with our modified TermDeleter that will call the right function
        auto result = PyTerm(t);
        check_for_errors();
        return result;
    }, "Create a rigid term", py::arg("sym"), py::arg("arity"));

    // Term operations - Convert BOOL return values to Python bool
    m.def("term_ident", [](Term t1, Term t2) {
        reset_error_flag();
        auto result = term_ident(t1, t2) == TRUE;
        check_for_errors();
        return result;
    }, "Check if two terms are identical", py::arg("t1"), py::arg("t2"));

    m.def("copy_term", [](Term t) {
        reset_error_flag();
        auto result = copy_term(t);
        check_for_errors();
        return result;
    }, "Create a copy of a term", py::arg("t"));

    m.def("is_term", [](Term t, const std::string& sym, int arity) {
        reset_error_flag();
        auto result = is_term(t, (char*)sym.c_str(), arity) == TRUE;
        check_for_errors();
        return result;
    }, "Check if a term has the given symbol and arity", 
       py::arg("t"), py::arg("sym"), py::arg("arity"));

    m.def("is_constant", [](Term t, const std::string& sym) {
        reset_error_flag();
        auto result = is_constant(t, (char*)sym.c_str()) == TRUE;
        check_for_errors();
        return result;
    }, "Check if a term is a constant with the given symbol", 
       py::arg("t"), py::arg("sym"));

    m.def("term_to_string", [](Term t) {
        reset_error_flag();
        auto result = term_to_string_cpp(t);
        check_for_errors();
        return result;
    }, "Convert a term to a string", py::arg("t"));

    m.def("ground_term", [](Term t) {
        reset_error_flag();
        auto result = ground_term(t) == TRUE;
        check_for_errors();
        return result;
    }, "Check if a term is ground (no variables)", py::arg("t"));

    m.def("term_depth", [](Term t) {
        reset_error_flag();
        auto result = term_depth(t);
        check_for_errors();
        return result;
    }, "Calculate the depth of a term", py::arg("t"));

    m.def("symbol_count", [](Term t) {
        reset_error_flag();
        auto result = symbol_count(t);
        check_for_errors();
        return result;
    }, "Count the number of symbols in a term", py::arg("t"));

    m.def("occurs_in", [](Term t1, Term t2) {
        reset_error_flag();
        auto result = occurs_in(t1, t2) == TRUE;
        check_for_errors();
        return result;
    }, "Check if one term occurs in another", py::arg("t1"), py::arg("t2"));

    // Term construction helpers - use copy_term to avoid double-free issues
    m.def("build_binary_term", [](const std::string& sym, Term a1, Term a2) {
        reset_error_flag();
        int sn = str_to_sn((char*)sym.c_str(), 2);
        // Make copies of arguments to avoid ownership issues
        Term arg1 = copy_term(a1);
        Term arg2 = copy_term(a2);
        auto result = build_binary_term(sn, arg1, arg2);
        check_for_errors();
        return result;
    }, "Build a binary term", py::arg("sym"), py::arg("a1"), py::arg("a2"));

    m.def("build_unary_term", [](const std::string& sym, Term a) {
        reset_error_flag();
        int sn = str_to_sn((char*)sym.c_str(), 1);
        // Make a copy of the argument to avoid ownership issues
        Term arg = copy_term(a);
        auto result = build_unary_term(sn, arg);
        check_for_errors();
        return result;
    }, "Build a unary term", py::arg("sym"), py::arg("a"));

    // Utility functions - use copy_term to avoid double-free issues
    m.def("term0", [](const std::string& sym) {
        reset_error_flag();
        auto result = term0((char*)sym.c_str());
        check_for_errors();
        return result;
    }, "Create a constant term", py::arg("sym"));

    m.def("term1", [](const std::string& sym, Term arg) {
        reset_error_flag();
        // Make a copy of the argument to avoid ownership issues
        Term arg_copy = copy_term(arg);
        auto result = term1((char*)sym.c_str(), arg_copy);
        check_for_errors();
        return result;
    }, "Create a unary term", py::arg("sym"), py::arg("arg"));

    m.def("term2", [](const std::string& sym, Term arg1, Term arg2) {
        reset_error_flag();
        // Make copies of arguments to avoid ownership issues
        Term arg1_copy = copy_term(arg1);
        Term arg2_copy = copy_term(arg2);
        auto result = term2((char*)sym.c_str(), arg1_copy, arg2_copy);
        check_for_errors();
        return result;
    }, "Create a binary term", py::arg("sym"), py::arg("arg1"), py::arg("arg2"));

    m.def("build_term", [](const std::string& sym, const std::vector<Term>& args) {
        reset_error_flag();
        int arity = args.size();
        Term t = get_rigid_term((char*)sym.c_str(), arity);
        for (int i = 0; i < arity; i++) {
            // Make a copy of each argument to avoid ownership issues
            ARG(t, i) = copy_term(args[i]);
        }
        check_for_errors();
        return t;
    }, "Create an n-ary term", py::arg("sym"), py::arg("args"));

    // Conversion functions
    m.def("nat_to_term", [](int n) {
        reset_error_flag();
        auto result = nat_to_term(n);
        check_for_errors();
        return result;
    }, "Convert a natural number to a term", py::arg("n"));

    m.def("int_to_term", [](int i) {
        reset_error_flag();
        auto result = int_to_term(i);
        check_for_errors();
        return result;
    }, "Convert an integer to a term", py::arg("i"));

    // Overloaded bool_to_term to handle both BOOL enum and Python boolean
    m.def("bool_to_term", [](BOOL val) {
        reset_error_flag();
        auto result = bool_to_term(val);
        check_for_errors();
        return result;
    }, "Convert a boolean to a term", py::arg("val"));
    
    m.def("bool_to_term", [](bool val) {
        reset_error_flag();
        auto result = bool_to_term(val ? TRUE : FALSE);
        check_for_errors();
        return result;
    }, "Convert a Python boolean to a term", py::arg("val"));

    m.def("term_to_int", [](Term t) {
        reset_error_flag();
        int result;
        if (term_to_int(t, &result)) {
            check_for_errors();
            return result;
        } else {
            throw py::value_error("Term cannot be converted to int");
        }
    }, "Convert a term to an integer", py::arg("t"));

    m.def("term_to_bool", [](Term t) {
        reset_error_flag();
        BOOL result;
        if (term_to_bool(t, &result)) {
            check_for_errors();
            return result == TRUE;
        } else {
            throw py::value_error("Term cannot be converted to bool");
        }
    }, "Convert a term to a boolean", py::arg("t"));

    // Add function to expose symbol table state
    m.def("get_symbol_table_state", []() {
        reset_error_flag();
        std::map<std::string, int> symbol_state;
        
        // Get all symbols and their arities
        for (int i = 1; i <= greatest_symnum(); i++) {
            if (sn_to_str(i)) {
                std::string name = sn_to_str(i);
                int arity = sn_to_arity(i);
                std::string key = name + "/" + std::to_string(arity);
                symbol_state[key] = i;
            }
        }
        
        check_for_errors();
        return symbol_state;
    }, "Get the current state of the symbol table");
}

// For backward compatibility with direct module import
PYBIND11_MODULE(term, m) {
    init_term_module(m);
} 