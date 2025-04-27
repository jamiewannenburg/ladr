#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <string>
#include "../common/error_handling.hpp"

// Ensure C linkage for LADR headers
extern "C" {
    #include "../../../ladr/memory.h"
}

namespace py = pybind11;
using namespace ladr;

// Initialize LADR memory system 
// This function must be called before any other LADR function
void init_ladr_memory() {
    // First disable memory checks completely to avoid any issues
    disable_max_megs();
    
    // If we want to use a memory limit, uncomment this line instead:
    // set_max_megs(5000);  // Allow up to 5GB of memory
}

// Function to initialize the memory module
void init_memory_module(py::module_& m) {
    m.def("set_max_megs", [](int megs) {
        set_max_megs(megs);
    }, "Set the memory limit in megabytes. 0 for unlimited.");

    // Memory status functions
    m.def("megs_malloced", []() {
        return megs_malloced();
    }, "Get the number of megabytes allocated");

    m.def("report_megabytes_malloced", [](double megs) {
        return megs_malloced() > megs;
    }, "Check if memory usage exceeds the given megabytes");

    m.def("megabytes_malloced", []() {
        return megs_malloced();
    }, "Get the number of megabytes allocated");

    m.def("bytes_malloced", []() {
        long long result = bytes_palloced();
        return result;
    }, "Get the number of bytes that have been allocated by palloc");
}

// For backward compatibility with direct module import
PYBIND11_MODULE(memory, m) {
    m.doc() = "Python bindings for LADR memory module";

    // Register error handling
    register_error_handling(m);

    // Initialize the memory system on module import
    // NOTE: This happens automatically when this module is imported
    init_ladr_memory();

    // Add function to manually initialize memory if needed
    m.def("init_memory", []() {
        // Initialize memory
        init_ladr_memory();
    }, "Initialize or reset the LADR memory system");

    // Add function to control memory limit
    m.def("set_memory_limit", [](int megs) {
        // Reset any previous error flags
        reset_error_flag();
        
        // Set memory limit
        if (megs <= 0) {
            disable_max_megs();
        } else {
            set_max_megs(megs);
            enable_max_megs();
        }
    }, "Set memory limit in megabytes (0 or negative to disable limit)", py::arg("megs"));

    // Add function to get current memory usage
    m.def("get_memory_usage", []() {
        return megs_malloced();
    }, "Get the number of megabytes that have been allocated");

    // Initialize the rest of the module
    init_memory_module(m);
} 