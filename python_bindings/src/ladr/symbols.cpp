#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <string>
#include "glist.hpp"
#include "common/conversions.hpp"

// Ensure C linkage for all the LADR headers
extern "C" {
    #include "../../../ladr/header.h"
    #include "../../../ladr/symbols.h"
}

namespace py = pybind11;

// Initialize the symbol table only once
static bool symbol_table_initialized = false;

void init_symbol_table() {
    if (!symbol_table_initialized) {
        // Initialize LADR packages including the symbol table
        declare_base_symbols();
        declare_standard_symbols();
        
        symbol_table_initialized = true;
    }
}

// Function to initialize the symbols module
void init_symbols_module(py::module_& m) {
    // Define the SymbolType enum
    py::enum_<Symbol_type>(m, "SymbolType")
        .value("UNSPECIFIED", UNSPECIFIED_SYMBOL)
        .value("FUNCTION", FUNCTION_SYMBOL)
        .value("RELATION", PREDICATE_SYMBOL)
        .export_values();

    // Define the ParseType enum
    py::enum_<Parsetype>(m, "ParseType")
        .value("NOTHING_SPECIAL", NOTHING_SPECIAL)
        .value("INFIX", INFIX)
        .value("INFIX_LEFT", INFIX_LEFT)
        .value("INFIX_RIGHT", INFIX_RIGHT)
        .value("PREFIX", PREFIX)
        .value("PREFIX_PAREN", PREFIX_PAREN)
        .value("POSTFIX", POSTFIX)
        .value("POSTFIX_PAREN", POSTFIX_PAREN)
        .export_values();

    // Define the UnifTheory enum
    py::enum_<Unif_theory>(m, "UnifTheory")
        .value("NONE", EMPTY_THEORY)
        .value("COMM", COMMUTE)
        .value("AC", ASSOC_COMMUTE)
        .export_values();

    // Define the LrpoStatus enum
    py::enum_<Lrpo_status>(m, "LrpoStatus")
        .value("LRPO_NONE", LRPO_LR_STATUS)
        .value("LRPO_MUL", LRPO_MULTISET_STATUS)
        .export_values();

    // Define the VariableStyle enum
    py::enum_<Variable_style>(m, "VariableStyle")
        .value("STANDARD", STANDARD_STYLE)
        .value("PROLOG", PROLOG_STYLE)
        .value("INTEGER", INTEGER_STYLE)
        .export_values();

    // Define constants
    m.attr("MAX_SYM") = MAX_SYM;
    m.attr("MAX_ARITY") = MAX_ARITY;
    m.attr("MAX_NAME") = MAX_NAME;
    m.attr("MIN_PRECEDENCE") = MIN_PRECEDENCE;
    m.attr("MAX_PRECEDENCE") = MAX_PRECEDENCE;

    // Symbol table initialization functions
    m.def("declare_base_symbols", &declare_base_symbols, "Declare base symbols");
    m.def("declare_standard_symbols", &declare_standard_symbols, "Declare standard symbols");

    // Symbol lookup and conversion functions
    m.def("str_to_sn", [](const std::string& str, int arity) {
        return str_to_sn((char*)str.c_str(), arity);
    }, "Convert string to symbol number", py::arg("str"), py::arg("arity"));

    m.def("sn_to_str", [](int symnum) {
        char* str = sn_to_str(symnum);
        std::string result(str);
        return result;
    }, "Convert symbol number to string", py::arg("symnum"));

    // Parse type functions
    m.def("set_parse_type", [](const std::string& str, int precedence, Parsetype type) {
        set_parse_type((char*)str.c_str(), precedence, type);
    }, "Set parse type for a symbol", 
       py::arg("str"), py::arg("precedence"), py::arg("type"));

    m.def("clear_parse_type", [](const std::string& str) {
        clear_parse_type((char*)str.c_str());
    }, "Clear parse type for a symbol", py::arg("str"));

    // Symbol property functions
    m.def("sn_to_arity", &sn_to_arity, "Get arity of a symbol", py::arg("symnum"));
    m.def("sn_to_occurrences", &sn_to_occurrences, "Get occurrences of a symbol", py::arg("symnum"));
    m.def("is_symbol", [](int symnum, const std::string& str, int arity) {
        return is_symbol(symnum, (char*)str.c_str(), arity) == TRUE;
    }, "Check if symbol matches string and arity",
       py::arg("symnum"), py::arg("str"), py::arg("arity"));

    // Symbol type functions
    m.def("set_symbol_type", &set_symbol_type, "Set symbol type", 
          py::arg("symnum"), py::arg("type"));
    m.def("get_symbol_type", &get_symbol_type, "Get symbol type", py::arg("symnum"));
    m.def("function_symbol", [](int symnum) {
        return function_symbol(symnum) == TRUE;
    }, "Check if symbol is a function", py::arg("symnum"));
    m.def("relation_symbol", [](int symnum) {
        return relation_symbol(symnum) == TRUE;
    }, "Check if symbol is a relation", py::arg("symnum"));

    // Variable style functions
    m.def("set_variable_style", &set_variable_style, "Set variable style", py::arg("style"));
    m.def("variable_style", &variable_style, "Get variable style");
    m.def("variable_name", [](const std::string& s) {
        return variable_name((char*)s.c_str()) == TRUE;
    }, "Check if string is a variable name", py::arg("s"));

    // Skolem functions
    m.def("set_skolem", &set_skolem, "Set symbol as skolem", py::arg("symnum"));
    m.def("is_skolem", [](int symnum) {
        return is_skolem(symnum) == TRUE;
    }, "Check if symbol is skolem", py::arg("symnum"));
    m.def("next_skolem_symbol", &next_skolem_symbol, "Get next skolem symbol", py::arg("arity"));

    // Lexicographic ordering functions
    m.def("set_lex_val", &set_lex_val, "Set lexicographic value", 
          py::arg("symnum"), py::arg("lex_val"));
    m.def("sn_to_lex_val", &sn_to_lex_val, "Get lexicographic value", py::arg("symnum"));
    m.def("sym_precedence", &sym_precedence, "Compare symbol precedence",
          py::arg("symnum_1"), py::arg("symnum_2"));

    // Knuth-Bendix ordering functions
    m.def("set_kb_weight", &set_kb_weight, "Set Knuth-Bendix weight",
          py::arg("symnum"), py::arg("weight"));
    m.def("sn_to_kb_wt", &sn_to_kb_wt, "Get Knuth-Bendix weight", py::arg("symnum"));

    // LRPO functions
    m.def("set_lrpo_status", &set_lrpo_status, "Set LRPO status",
          py::arg("symnum"), py::arg("status"));
    m.def("sn_to_lrpo_status", &sn_to_lrpo_status, "Get LRPO status", py::arg("symnum"));

    // Associative-commutative functions
    m.def("set_assoc_comm", [](const std::string& str, bool set) {
        set_assoc_comm((char*)str.c_str(), set ? TRUE : FALSE);
    }, "Set associative-commutative property", py::arg("str"), py::arg("set"));
    m.def("set_commutative", [](const std::string& str, bool set) {
        set_commutative((char*)str.c_str(), set ? TRUE : FALSE);
    }, "Set commutative property", py::arg("str"), py::arg("set"));
    m.def("is_assoc_comm", [](int sn) {
        return is_assoc_comm(sn) == TRUE;
    }, "Check if symbol is associative-commutative", py::arg("sn"));
    m.def("is_commutative", [](int sn) {
        return is_commutative(sn) == TRUE;
    }, "Check if symbol is commutative", py::arg("sn"));
}

PYBIND11_MODULE(symbols, m) {
    m.doc() = "Python bindings for LADR symbols module";
    init_symbols_module(m);
} 