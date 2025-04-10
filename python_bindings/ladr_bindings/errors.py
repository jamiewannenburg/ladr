"""Custom exceptions for LADR Python bindings."""

# Default exit code for fatal errors
DEFAULT_FATAL_EXIT_CODE = 1

class FatalError(Exception):
    """Exception raised when a fatal error occurs in LADR.
    
    This exception corresponds to fatal errors that would normally
    cause program termination in the C implementation.
    """
    def __init__(self, message, exit_code=DEFAULT_FATAL_EXIT_CODE):
        super().__init__(message)
        self.exit_code = exit_code 