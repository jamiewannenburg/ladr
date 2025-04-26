#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <string>
#include <stdexcept>

// Forward declarations for our custom functions in fatal_wrapper.c
extern "C" {
    int has_error_occurred(void);
    void reset_error_flag(void);
    char* get_fatal_error_message(void);
    void suppress_error_messages(int flag);
}

namespace py = pybind11;

// Custom exception for LADR fatal errors
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

// Function decorator that catches LADR fatal errors and turns them into Python exceptions
template <typename Func>
auto wrap_ladr_errors(Func&& func) {
    return [func = std::forward<Func>(func)](auto&&... args) {
        // Reset any previous error flags
        reset_error_flag();
        
        // Call the function
        auto result = func(std::forward<decltype(args)>(args)...);
        
        // Check if an error occurred
        check_for_ladr_error();
        
        return result;
    };
}

PYBIND11_MODULE(error_handler, m) {
    m.doc() = "Python bindings for LADR error handling";

    // Register the exception
    py::register_exception<LadrFatalError>(m, "LadrFatalError");

    // Add function to control error message printing
    m.def("suppress_error_messages", &suppress_error_messages, 
          "Control whether fatal error messages are printed to stdout/stderr (1=suppress, 0=print)",
          py::arg("flag"));

    // Add function to check for errors
    m.def("check_for_errors", []() {
        check_for_ladr_error();
    }, "Check if a LADR fatal error has occurred and raise an exception if so");

    // Add function to reset error flags
    m.def("reset_error_flags", []() {
        reset_error_flag();
    }, "Reset the LADR error flags");

    // Expose decorator for use with other modules
    m.def("catch_fatal_errors", [](py::function func) {
        return py::cpp_function([func](py::args args, py::kwargs kwargs) {
            // Reset any previous error flags
            reset_error_flag();
            
            // Call the function
            py::object result = func(*args, **kwargs);
            
            // Check if an error occurred
            check_for_ladr_error();
            
            return result;
        });
    }, "Wrap a function to catch LADR fatal errors and convert them to exceptions");
} 