#pragma once

#include <pybind11/pybind11.h>
#include <pybind11/functional.h>
#include <string>
#include <stdexcept>

// Ensure C linkage for error functions from fatal_wrapper.c
extern "C" {
    int has_error_occurred(void);
    void reset_error_flag(void);
    char* get_fatal_error_message(void);
    void suppress_error_messages(int flag);
}

namespace ladr {

namespace py = pybind11;

// Custom exception for LADR fatal errors
class LadrFatalError : public std::runtime_error {
public:
    LadrFatalError(const std::string& message) : std::runtime_error(message) {}
};

// Function that checks for LADR errors and throws an exception if found
inline void check_for_errors() {
    if (has_error_occurred()) {
        // Get the error message
        std::string error_message = get_fatal_error_message();
        
        // Reset the error flag for future calls
        reset_error_flag();
        
        // Throw an exception with the error message
        throw LadrFatalError(error_message);
    }
}

// Reset error flags before a function call
inline void reset_errors() {
    reset_error_flag();
}

// Macro to handle error checking before and after a function call
#define LADR_CALL(expr) \
    do { \
        reset_error_flag(); \
        auto result = (expr); \
        check_for_errors(); \
        return result; \
    } while(0)

// Macro for void functions
#define LADR_CALL_VOID(expr) \
    do { \
        reset_error_flag(); \
        (expr); \
        check_for_errors(); \
        return; \
    } while(0)

// Function to wrap a Python callable in a try/catch block for LADR fatal errors
inline py::function wrap_python_function(py::function f) {
    return py::cpp_function([f](py::args args, py::kwargs kwargs) {
        try {
            // Reset the error flag before calling the function
            reset_error_flag();
            
            // Call the function
            py::object result = f(*args, **kwargs);
            
            // Check for errors after the function call
            check_for_errors();
            
            return result;
        } catch (py::error_already_set &e) {
            // Pass through Python exceptions
            throw;
        } catch (LadrFatalError &e) {
            // Re-throw LadrFatalError
            throw;
        } catch (std::exception &e) {
            // Convert other C++ exceptions to Python exceptions
            throw std::runtime_error(e.what());
        }
    });
}

// Register the error handling module
inline void register_error_handling(py::module_& m) {
    // Register the exception
    py::register_exception<LadrFatalError>(m, "LadrFatalError");
    
    // Export basic error handling functions
    m.def("has_error_occurred", &has_error_occurred, "Check if a LADR fatal error has occurred");
    m.def("reset_error_flag", &reset_error_flag, "Reset the LADR fatal error flag");
    m.def("get_fatal_error_message", &get_fatal_error_message, "Get the LADR fatal error message");
    m.def("suppress_error_messages", &suppress_error_messages, "Suppress LADR error messages");
    
    // Export utility functions
    m.def("check_for_errors", &check_for_errors, 
          "Check for LADR errors and throw an exception if found");
    
    m.def("catch_fatal_errors", &wrap_python_function,
          "Wrap a function to catch LADR fatal errors and convert them to exceptions");
}

} // namespace ladr 