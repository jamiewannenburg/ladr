#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <string>
#include <vector>
#include <memory>

// Ensure C linkage for all the LADR headers
extern "C" {
    #include "../../ladr/parse.h"
    #include "../../ladr/term.h"
    #include "../../ladr/listterm.h"
    #include "../../ladr/string.h"
    #include "../../ladr/symbols.h"
    #include "../../ladr/strbuf.h"
}

namespace py = pybind11;

// Custom string conversion functions
std::string term_to_string_cpp(Term t) {
    char* str = term_to_string(t);
    std::string result(str);
    free(str);
    return result;
}

// Custom wrapper for parse_term_from_string to handle C string conversion
Term parse_term_from_string_cpp(const std::string& s) {
    return parse_term_from_string((char*)s.c_str());
}

// Custom wrapper for sb_write_term to return a string
std::string write_term_to_string(Term t) {
    String_buf sb = get_string_buf();
    sb_write_term(sb, t);
    
    // Convert string buffer to string
    char* c_str = sb_to_malloc_string(sb);
    std::string result(c_str);
    free(c_str);
    
    zap_string_buf(sb);
    return result;
}

// Custom wrapper for declare_parse_type
void declare_parse_type_cpp(const std::string& str, int precedence, Parsetype type) {
    declare_parse_type((char*)str.c_str(), precedence, type);
}

// Enum for parse types - use integer values to avoid including private header
enum class ParseType {
    INFIX_LEFT = 0,
    INFIX_RIGHT = 1,
    INFIX = 2,
    PREFIX = 3,
    POSTFIX = 4,
    PREFIX_PAREN = 5,
    POSTFIX_PAREN = 6,
    ORDINARY = 7
};

PYBIND11_MODULE(parse, m) {
    m.doc() = "Python bindings for LADR parse module";

    // Define the ParseType enum
    py::enum_<ParseType>(m, "ParseType")
        .value("INFIX_LEFT", ParseType::INFIX_LEFT, "Left-associative infix operator")
        .value("INFIX_RIGHT", ParseType::INFIX_RIGHT, "Right-associative infix operator")
        .value("INFIX", ParseType::INFIX, "Non-associative infix operator")
        .value("PREFIX", ParseType::PREFIX, "Prefix operator")
        .value("POSTFIX", ParseType::POSTFIX, "Postfix operator")
        .value("PREFIX_PAREN", ParseType::PREFIX_PAREN, "Prefix operator with parentheses")
        .value("POSTFIX_PAREN", ParseType::POSTFIX_PAREN, "Postfix operator with parentheses")
        .value("ORDINARY", ParseType::ORDINARY, "Ordinary symbol")
        .export_values();

    // Parse functions
    m.def("parse_term_from_string", &parse_term_from_string_cpp,
        "Parse a term from a string", py::arg("s"));

    m.def("write_term_to_string", &write_term_to_string,
        "Convert a term to a string representation", py::arg("t"));

    m.def("declare_parse_type", [](const std::string& sym, int precedence, ParseType type) {
        declare_parse_type_cpp(sym, precedence, static_cast<Parsetype>(static_cast<int>(type)));
    }, "Declare the parsing type for a symbol",
       py::arg("sym"), py::arg("precedence"), py::arg("type"));

    m.def("declare_standard_parse_types", &declare_standard_parse_types,
        "Declare the standard parsing types for common symbols");

    m.def("declare_quantifier_precedence", &declare_quantifier_precedence,
        "Set the precedence for quantifiers", py::arg("prec"));

    m.def("set_cons_char", [](char c) {
        set_cons_char(c);
    }, "Set the list constructor character (default '.')", py::arg("c"));

    m.def("get_cons_char", &get_cons_char,
        "Get the list constructor character");

    m.def("set_quote_char", [](char c) {
        set_quote_char(c);
    }, "Set the quote character (default '\"')", py::arg("c"));

    m.def("get_quote_char", &get_quote_char,
        "Get the quote character");

    m.def("parenthesize", [](bool setting) {
        parenthesize(setting ? TRUE : FALSE);
    }, "Set whether to add parentheses in all terms for output",
       py::arg("setting"));

    m.def("check_for_illegal_symbols", [](bool setting) {
        check_for_illegal_symbols(setting ? TRUE : FALSE);
    }, "Set whether to check for illegal symbols during parsing",
       py::arg("setting"));

    m.def("simple_parse", [](bool setting) {
        simple_parse(setting ? TRUE : FALSE);
    }, "Set whether to use simple parsing (no operator precedence)",
       py::arg("setting"));

    m.def("translate_neg_equalities", [](bool flag) {
        translate_neg_equalities(flag ? TRUE : FALSE);
    }, "Set whether to translate negative equalities to ~(a=b) form",
       py::arg("flag"));

    m.def("ordinary_constant_string", [](const std::string& s) {
        return ordinary_constant_string((char*)s.c_str()) == TRUE;
    }, "Check if a string is an ordinary constant",
       py::arg("s"));

    m.def("split_string", [](const std::string& onlys) {
        Plist result = split_string((char*)onlys.c_str());
        
        // Convert Plist to Python list of strings
        std::vector<std::string> strings;
        Plist p;
        for (p = result; p != NULL; p = p->next) {
            char* str = (char*)p->v;
            strings.push_back(std::string(str));
        }
        
        // Free the Plist
        zap_plist(result);
        
        return strings;
    }, "Split a string into a list of strings based on special characters",
       py::arg("onlys"));

    m.def("process_quoted_symbol", [](const std::string& str) {
        char* result = process_quoted_symbol((char*)str.c_str());
        std::string cpp_result(result);
        free(result);
        return cpp_result;
    }, "Process a quoted symbol, handling escape sequences",
       py::arg("str"));
} 