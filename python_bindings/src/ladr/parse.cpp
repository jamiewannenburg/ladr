#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <string>

// Ensure C linkage for all the LADR headers
extern "C" {
    #include "../../../ladr/header.h"
    #include "../../../ladr/parse.h"
    #include "../../../ladr/term.h"
    #include "../../../ladr/symbols.h"  // For declare_base_symbols TODO implement own binding
    #include "../../../ladr/top_input.h" // For init_standard_ladr TODO own binding
}

// Error handling functions
extern "C" {
    int has_error_occurred(void);
    void reset_error_flag(void);
    char* get_fatal_error_message(void);
}

namespace py = pybind11;

// Import LadrFatalError from error_handler.cpp
class LadrFatalError : public std::runtime_error {
public:
    LadrFatalError(const std::string& message) : std::runtime_error(message) {}
};

// Function that checks for LADR errors and throws an exception if found
void check_for_ladr_error() {
    if (has_error_occurred()) {
        // Get the error message
        std::string error_message = get_fatal_error_message();
        
        // Reset the error flag for future calls
        reset_error_flag();
        
        // Throw an exception with the error message
        throw LadrFatalError(error_message);
    }
}

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
        declare_base_symbols();
        declare_standard_parse_types();
        translate_neg_equalities(TRUE);
        
        parser_initialized = true;
    }
}

PYBIND11_MODULE(parse, m) {
    m.doc() = "Python bindings for LADR parse module";

    // Register the exception
    py::register_exception<LadrFatalError>(m, "LadrFatalError");
    
    // Initialize the parser when the module is loaded
    init_ladr_parser(); // TODO: Add package options for this

    // Add function to manually initialize the parser if needed
    m.def("init_parser", []() {
        // Reset any previous error flags
        reset_error_flag();
        
        // Initialize the parser
        init_ladr_parser();
        
        // Check for errors
        check_for_ladr_error();
    }, "Initialize or reset the LADR parser");

    // Expose parse_term_from_string function with error handling
    m.def("parse_term_from_string", [](const std::string& s) {
        // Reset any previous error flags
        reset_error_flag();
        
        // Call the C function
        Term t = parse_term_from_string((char*)s.c_str());
        
        // Check for errors
        check_for_ladr_error();
        
        if (t == NULL) {
            throw py::value_error("Failed to parse term from string");
        }
        
        // Return a PyTerm with our TermDeleter that handles memory management
        return PyTerm(t);
    }, "Parse a term from a string", py::arg("s"));
} 