#include "fatal.h"
// Custom exception for LADR fatal errors

extern "C" {
    void throw_ladr_fatal_error(char *message, int exit_code)
    {
            // convert c-string message to std::string
            std::string error_message = message;
            
            // Throw a runtime_error with the error message
            throw LadrFatalError(error_message, exit_code);
    }
}
#ifdef __cplusplus

// Overload for std::string comming from C++
void throw_ladr_fatal_error(std::string error_message, int exit_code)
{
        
        // Throw a runtime_error with the error message
        throw LadrFatalError(error_message, exit_code);
}
#endif
