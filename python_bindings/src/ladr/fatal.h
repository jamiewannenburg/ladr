#ifndef FATAL_H
#define FATAL_H

#ifdef __cplusplus
#include <stdexcept>
#include <string>
// Custom exception for LADR fatal errors
class LadrFatalError : public std::runtime_error {
public:
    LadrFatalError(const std::string& message, int exit_code) 
        : std::runtime_error("Exit code " + std::to_string(exit_code) + ": " + message) {}
};

// Throw a LADR fatal error in c++
void throw_ladr_fatal_error(std::string error_message, int exit_code);

extern "C" {
#endif

// Throw a LADR fatal error in c
void throw_ladr_fatal_error(char *message, int exit_code);

#ifdef __cplusplus
}
#endif

#endif // FATAL_H