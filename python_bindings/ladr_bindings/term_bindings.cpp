#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

// Ensure C linkage for all the LADR headers
extern "C" {
    #include "../../ladr/term.h"
    #include "../../ladr/symbols.h"
}

namespace py = pybind11;

// Custom string conversion functions
std::string term_to_string_cpp(Term t) {
    char* str = term_to_string(t);
    std::string result(str);
    free(str);
    return result;
}

// Custom deleter for Term objects
struct TermDeleter {
    void operator()(Term t) const {
        if (t) zap_term(t);
    }
};

using PyTerm = std::unique_ptr<struct term, TermDeleter>;

PYBIND11_MODULE(term, m) {
    m.doc() = "Python bindings for LADR term module";

    // Define constants
    m.attr("MAX_VARS") = MAX_VARS;
    m.attr("MAX_VNUM") = MAX_VNUM;
    m.attr("MAX_VAR") = MAX_VAR;
    m.attr("MAX_SYM") = MAX_SYM;
    m.attr("MAX_ARITY") = MAX_ARITY;

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

    // Term creation functions
    m.def("get_variable_term", [](int var_num) {
        if (var_num < 0 || var_num > MAX_VAR) {
            throw py::value_error("Variable number out of range [0, MAX_VAR]");
        }
        return get_variable_term(var_num);
    }, "Create a variable term", py::arg("var_num"));

    m.def("get_rigid_term", [](const std::string& sym, int arity) {
        if (arity < 0 || arity > MAX_ARITY) {
            throw py::value_error("Arity out of range [0, MAX_ARITY]");
        }
        return get_rigid_term((char*)sym.c_str(), arity);
    }, "Create a rigid term", py::arg("sym"), py::arg("arity"));

    // Term operations
    m.def("term_ident", &term_ident, "Check if two terms are identical", 
          py::arg("t1"), py::arg("t2"));

    m.def("copy_term", &copy_term, "Create a copy of a term", py::arg("t"));

    m.def("is_term", [](Term t, const std::string& sym, int arity) {
        return is_term(t, (char*)sym.c_str(), arity);
    }, "Check if a term has the given symbol and arity", 
       py::arg("t"), py::arg("sym"), py::arg("arity"));

    m.def("is_constant", [](Term t, const std::string& sym) {
        return is_constant(t, (char*)sym.c_str());
    }, "Check if a term is a constant with the given symbol", 
       py::arg("t"), py::arg("sym"));

    m.def("term_to_string", &term_to_string_cpp, "Convert a term to a string", 
          py::arg("t"));

    m.def("ground_term", &ground_term, "Check if a term is ground (no variables)", 
          py::arg("t"));

    m.def("term_depth", &term_depth, "Calculate the depth of a term", 
          py::arg("t"));

    m.def("symbol_count", &symbol_count, "Count the number of symbols in a term", 
          py::arg("t"));

    m.def("occurs_in", &occurs_in, "Check if one term occurs in another", 
          py::arg("t1"), py::arg("t2"));

    // Term construction helpers - use copy_term to avoid double-free issues
    m.def("build_binary_term", [](const std::string& sym, Term a1, Term a2) {
        int sn = str_to_sn((char*)sym.c_str(), 2);
        // Make copies of arguments to avoid ownership issues
        Term arg1 = copy_term(a1);
        Term arg2 = copy_term(a2);
        return build_binary_term(sn, arg1, arg2);
    }, "Build a binary term", py::arg("sym"), py::arg("a1"), py::arg("a2"));

    m.def("build_unary_term", [](const std::string& sym, Term a) {
        int sn = str_to_sn((char*)sym.c_str(), 1);
        // Make a copy of the argument to avoid ownership issues
        Term arg = copy_term(a);
        return build_unary_term(sn, arg);
    }, "Build a unary term", py::arg("sym"), py::arg("a"));

    // Utility functions - use copy_term to avoid double-free issues
    m.def("term0", [](const std::string& sym) {
        return term0((char*)sym.c_str());
    }, "Create a constant term", py::arg("sym"));

    m.def("term1", [](const std::string& sym, Term arg) {
        // Make a copy of the argument to avoid ownership issues
        Term arg_copy = copy_term(arg);
        return term1((char*)sym.c_str(), arg_copy);
    }, "Create a unary term", py::arg("sym"), py::arg("arg"));

    m.def("term2", [](const std::string& sym, Term arg1, Term arg2) {
        // Make copies of arguments to avoid ownership issues
        Term arg1_copy = copy_term(arg1);
        Term arg2_copy = copy_term(arg2);
        return term2((char*)sym.c_str(), arg1_copy, arg2_copy);
    }, "Create a binary term", py::arg("sym"), py::arg("arg1"), py::arg("arg2"));

    // Conversion functions
    m.def("nat_to_term", &nat_to_term, "Convert a natural number to a term", 
          py::arg("n"));

    m.def("int_to_term", &int_to_term, "Convert an integer to a term", 
          py::arg("i"));

    // Overloaded bool_to_term to handle both BOOL enum and Python boolean
    m.def("bool_to_term", &bool_to_term, "Convert a boolean to a term", 
          py::arg("val"));
    
    m.def("bool_to_term", [](bool val) {
        return bool_to_term(val ? TRUE : FALSE);
    }, "Convert a Python boolean to a term", py::arg("val"));

    m.def("term_to_int", [](Term t) {
        int result;
        if (term_to_int(t, &result)) {
            return result;
        } else {
            throw py::value_error("Term cannot be converted to int");
        }
    }, "Convert a term to an integer", py::arg("t"));

    m.def("term_to_bool", [](Term t) {
        BOOL result;
        if (term_to_bool(t, &result)) {
            return result == TRUE ? true : false;
        } else {
            throw py::value_error("Term cannot be converted to bool");
        }
    }, "Convert a term to a boolean", py::arg("t"));
} 