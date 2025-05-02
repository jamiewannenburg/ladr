// Pybind11 and C++ standard headers
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <string>
#include <map>
#include <memory>
#include "strbuf.hpp"

// Add support for converting Python file objects to C FILE*
#ifdef _WIN32
  #include <io.h>
  #define dup _dup
  #define fdopen _fdopen
#else
  #include <unistd.h>  // for dup, fdopen
#endif
#include <cstdio>

// C headers for LADR
extern "C" {
    #include "../../../ladr/header.h"
    #include "../../../ladr/parse.h"
    #include "../../../ladr/term.h"
    #include "../../../ladr/symbols.h"  // For declare_base_symbols
    #include "../../../ladr/top_input.h" // For init_standard_ladr
}

namespace py = pybind11;

// Helper to convert a Python file-like object to a C FILE*
static FILE* to_c_file(py::object py_file, const char* mode) {
    // None -> stdin for read, stdout for write
    if (py_file.is_none())
        return (mode[0] == 'r') ? stdin : stdout;
    if (!py::hasattr(py_file, "fileno"))
        throw py::type_error("File object must have fileno() method");
    int fd = py_file.attr("fileno")().cast<int>();
    int dup_fd = dup(fd);
    FILE* f = fdopen(dup_fd, mode);
    if (!f)
        throw py::value_error("Failed to convert file descriptor to FILE*");
    return f;
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
        init_standard_ladr();  // This will call declare_base_symbols and declare_standard_parse_types
        
        // Additional parser settings
        translate_neg_equalities(TRUE);
        
        parser_initialized = true;
    }
}

// Function to initialize the parse module
void init_parse_module(py::module_& m) {
    
    // Add function to manually initialize the parser if needed
    m.def("init_parser", []() {
        init_ladr_parser();
    }, "Initialize or reset the LADR parser");

    // Expose parse_term_from_string function with error handling
    m.def("parse_term_from_string", [](const std::string& s) {
        // Call the C function
        Term t = parse_term_from_string((char*)s.c_str());
        // this is a hack
        // TODO just catch the fatal error
        if (t == NULL) {
            throw py::value_error("Failed to parse term from string");
        }
        // Return a PyTerm with our TermDeleter that handles memory management
        return PyTerm(t);
    }, "Parse a term from a string", py::arg("s"));

    // Add function to expose symbol table state
    m.def("get_symbol_table_state", []() {
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
        return symbol_state;
    }, "Get the current state of the symbol table");

    // Expose sread_term: read a Term from a Python buffer (io.BytesIO) with optional output file
    m.def("sread_term", [](py::object sb_obj, py::object out_file) {
        init_ladr_parser();
        
        // Create a new String_buf (don't try to cast Python -> C directly)
        String_buf sb = get_string_buf();
        
        // Extract data from BytesIO
        py::object val = sb_obj.attr("getvalue")();
        if (!py::isinstance<py::bytes>(val)) {
            throw py::type_error("Expected a BytesIO object with bytes content");
        }
        
        // Copy data to String_buf byte by byte
        std::string data = val.cast<std::string>();
        for (unsigned char c : data) {
            sb_append_char(sb, c);
        }
        
        // Pass to C function
        FILE *fout = to_c_file(out_file, "w");
        Term t = sread_term(sb, fout);
        zap_string_buf(sb);
        
        if (!out_file.is_none()) fclose(fout);
        if (!t)
            throw py::value_error("Failed to read term from buffer");
        return PyTerm(t);
    }, "Read a term from an io.BytesIO object with optional output file",
       py::arg("sb"), py::arg("out_file") = py::none());

    // Expose read_term: read a Term from Python file-like objects
    m.def("read_term", [](py::object in_file, py::object out_file) {
        init_ladr_parser();
        FILE *fin = to_c_file(in_file, "r");
        FILE *fout = to_c_file(out_file, "w");
        Term t = read_term(fin, fout);
        if (!in_file.is_none()) fclose(fin);
        if (!out_file.is_none()) fclose(fout);
        if (!t)
            throw py::value_error("Failed to read term from file-like object");
        return PyTerm(t);
    }, "Read a term from Python file-like objects",
       py::arg("in_file"), py::arg("out_file") = py::none());
}

// For backward compatibility with direct module import
PYBIND11_MODULE(parse, m) {
    m.doc() = "Python bindings for LADR parse module";
    
    // Initialize the parser when the module is loaded
    init_ladr_parser();
    init_parse_module(m);
} 