#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <string>
#include "../common/error_handling.hpp"

// Ensure C linkage for all the LADR headers
extern "C" {
    #include "../../../ladr/header.h"
    #include "../../../ladr/parse.h"
    #include "../../../ladr/term.h"
    #include "../../../ladr/symbols.h"  // For declare_base_symbols TODO implement own binding
    #include "../../../ladr/top_input.h" // For init_standard_ladr TODO own binding
}

namespace py = pybind11;
using namespace ladr;

// Import Term deleter from term.cpp
struct TermDeleter {
    void operator()(Term t) const {
        if (t == NULL) return;
        
        if (t->private_symbol >= 0) {
            // Variables don't need to be freed
            return;
        }
        
        // For regular terms, call zap_term
        zap_term(t);
    }
};

using PyTerm = std::unique_ptr<struct term, TermDeleter>;

// Initialize the parser only once
static bool parser_initialized = false;

void init_ladr_parser() {
    if (!parser_initialized) {
        // Initialize LADR packages including the parser
        init_standard_ladr();  // This will call declare_base_symbols and declare_standard_parse_types
        
        // Additional parser settings
        translate_neg_equalities(TRUE);
        
        parser_initialized = true;
    }
}

// Function to initialize the parse module
void init_parse_module(py::module_& m) {
    // Register error handling
    register_error_handling(m);
    
    // Add function to manually initialize the parser if needed
    m.def("init_parser", []() {
        reset_error_flag();
        init_ladr_parser();
        check_for_errors();
    }, "Initialize or reset the LADR parser");

    // Expose parse_term_from_string function with error handling
    m.def("parse_term_from_string", [](const std::string& s) {
        reset_error_flag();
        
        // Call the C function
        Term t = parse_term_from_string((char*)s.c_str());
        
        if (t == NULL) {
            throw py::value_error("Failed to parse term from string");
        }
        
        check_for_errors();
        // Return a PyTerm with our TermDeleter that handles memory management
        return PyTerm(t);
    }, "Parse a term from a string", py::arg("s"));

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
PYBIND11_MODULE(parse, m) {
    m.doc() = "Python bindings for LADR parse module";
    
    // Initialize the parser when the module is loaded
    init_ladr_parser();
    
    init_parse_module(m);
} 