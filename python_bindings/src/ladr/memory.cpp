#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <string>

// Ensure C linkage for LADR headers
extern "C" {
    #include "../../../ladr/memory.h"
}

// Error handling functions
extern "C" {
    int has_error_occurred(void);
    void reset_error_flag(void);
    char* get_fatal_error_message(void);
    void suppress_error_messages(int flag);
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

// Initialize LADR memory system 
// This function must be called before any other LADR function
void init_ladr_memory() {
    // First disable memory checks completely to avoid any issues
    disable_max_megs();
    
    // If we want to use a memory limit, uncomment this line instead:
    // set_max_megs(5000);  // Allow up to 5GB of memory
}

PYBIND11_MODULE(memory, m) {
    m.doc() = "Python bindings for LADR memory module";

    // Register the exception
    py::register_exception<LadrFatalError>(m, "LadrFatalError");

    // Initialize the memory system on module import
    // NOTE: This happens automatically when this module is imported
    init_ladr_memory();

    // Add function to manually initialize memory if needed
    m.def("init_memory", []() {
        // Reset any previous error flags
        reset_error_flag();
        
        // Initialize memory
        init_ladr_memory();
        
        // Check for errors
        check_for_ladr_error();
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
        
        // Check for errors
        check_for_ladr_error();
    }, "Set memory limit in megabytes (0 or negative to disable limit)", py::arg("megs"));

    // Add function to get current memory usage
    m.def("megs_malloced", []() {
        // Reset any previous error flags
        reset_error_flag();
        
        // Get memory usage
        int result = megs_malloced();
        
        // Check for errors
        check_for_ladr_error();
        
        return result;
    }, "Get the number of megabytes that have been allocated");
    
    // Add function to get bytes allocated
    m.def("bytes_palloced", []() {
        // Reset any previous error flags
        reset_error_flag();
        
        // Get bytes allocated
        int result = bytes_palloced();
        
        // Check for errors
        check_for_ladr_error();
        
        return result;
    }, "Get the number of bytes that have been allocated by palloc");
} 