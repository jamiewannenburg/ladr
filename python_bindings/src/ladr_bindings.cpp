#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <string>
//#include "../common/error_handling.hpp"
#include "ladr/fatal.h"
class LadrFatalError;

// Ensure C linkage for all the LADR headers
extern "C" {
    #include "../../ladr/header.h"
    #include "../../ladr/term.h"
    #include "../../ladr/parse.h"
    #include "../../ladr/symbols.h"
    #include "../../ladr/top_input.h"
    #include "ladr/fatal_wrapper.c"
}

namespace py = pybind11;
//using namespace ladr;

// Function declarations from other modules
void init_term_module(py::module_& m);
void init_parse_module(py::module_& m);
void init_glist_module(py::module_& m);
void init_memory_module(py::module_& m);
void init_error_handler_module(py::module_& m);

// Initialize the LADR system only once for all modules
static bool ladr_initialized = false;

void init_ladr_system() {
    if (!ladr_initialized) {
        // Initialize LADR packages
        init_standard_ladr();  // This will call declare_base_symbols and declare_standard_symbols
        translate_neg_equalities(TRUE);
        
        ladr_initialized = true;
    }
}

PYBIND11_MODULE(ladr_bindings, m) {
    m.doc() = "Combined Python bindings for LADR library";
    // Register the error handling 
    py::register_exception<LadrFatalError>(m, "LadrFatalError");
    // Initialize the LADR system when the module is loaded
    init_ladr_system();
    // Create submodules
    py::module_ term_m = m.def_submodule("term", "LADR term module");
    py::module_ parse_m = m.def_submodule("parse", "LADR parse module");
    py::module_ glist_m = m.def_submodule("glist", "LADR glist module");
    py::module_ memory_m = m.def_submodule("memory", "LADR memory module");

    // Initialize each submodule (with function calls from their respective modules
    init_term_module(term_m);
    init_parse_module(parse_m);
    init_glist_module(glist_m);
    init_memory_module(memory_m);
} 