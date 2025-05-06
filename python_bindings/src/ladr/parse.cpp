// Pybind11 and C++ standard headers
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <string>
#include <map>
#include <memory>
#include "strbuf.hpp"
#include "common/conversions.hpp"
#include "term.hpp"

// C headers for LADR
extern "C" {
    #include "../../../ladr/header.h"
    #include "../../../ladr/parse.h"
    #include "../../../ladr/term.h"
    //#include "../../../ladr/symbols.h"  // For declare_base_symbols
    //#include "../../../ladr/top_input.h" // For init_standard_ladr
}

namespace py = pybind11;

// Initialize the parser only once
static bool parser_initialized = false;

void init_ladr_parser() {
    if (!parser_initialized) {
        // Initialize LADR packages including the parser
        //init_standard_ladr();  // This will call declare_base_symbols and declare_standard_parse_types
        
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

    // Expose declare_parse_type function
    m.def("declare_parse_type", [](const std::string& str, int precedence, Parsetype pt) { // Parsetype from symbols.cpp
        declare_parse_type((char*)str.c_str(), precedence, pt);
    }, "Declare a parse type", py::arg("str"), py::arg("precedence"), py::arg("pt"));

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
    m.def("sread_term", [](py::object sb_obj, std::string fout_name) {
        //init_ladr_parser();
        FILE* fout = fopen(fout_name.c_str(), "w");
        if (!fout) {
            throw py::value_error("Failed to open output file");
        }
        // Create a new String_buf (don't try to cast Python -> C directly)
        String_buf sb = get_string_buf();
        // Extract data from BytesIO up to end_char is true or everything is read
        py::object read_source_func = sb_obj.attr("read");
        while (true) {
            // Read one byte at a time
            py::bytes bytes_read = read_source_func(1).cast<py::bytes>();
            std::string data = bytes_read.cast<std::string>();
            if (data.size() == 0) {
                break; // EOF reached
            }
            char b = data[0];
            sb_append_char(sb, b);
            if (end_char(b)) { // end_char is defined in parse.h
                break;
            }
        }
        
        Term t = sread_term(sb, fout);
        zap_string_buf(sb);
        
        fclose(fout);
        if (!t)
            throw py::value_error("Failed to read term from buffer");
        return PyTerm(t);
    }, "Read a term from an io.BytesIO object with optional output file",
       py::arg("sb"), py::arg("fout_name"));

    // Expose read_term: read a Term from Python file-like objects
    m.def("read_term", [](std::string fin_name, std::string fout_name) {
        FILE* fin = fopen(fin_name.c_str(), "r");
        if (!fin) {
            throw py::value_error("Failed to open input file");
        }
        FILE* fout = fopen(fout_name.c_str(), "w");
        if (!fout) {
            throw py::value_error("Failed to open output file");
        }
        Term t = read_term(fin, fout);
        fclose(fin);
        fclose(fout);
        if (!t)
            throw py::value_error("Failed to read term from file-like object");
        return PyTerm(t);
    }, "Read a term from Python file-like objects",
       py::arg("fin_name"), py::arg("fout_name"));
}

// For backward compatibility with direct module import
PYBIND11_MODULE(parse, m) {
    m.doc() = "Python bindings for LADR parse module";
    
    // Initialize the parser when the module is loaded
    //init_ladr_parser();
    init_parse_module(m);
} 